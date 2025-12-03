#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LENGTH 256            // giris dosyasi icin bir satirdaki karakter siniri
#define INITIAL_CAPACITY 26            // giris dosyasi icin sinir (gerekirse kendi kendini genisletiyor)
#define ANSI_COLOR_RESET "\033[0m  \n" // renkli loglar icin resetleyici
#define ANSI_COLOR_SET_24BIT_FG "  \033[38;2;%d;%d;%dm" // renkli loglar icin renk girdisi

static TaskParams* TASK_LIST = NULL; // gorevlerin tutuldugu liste
static int TASK_COUNT = 0;           // gorev listesinin eleman sayisi
static int CURR_TIME = 0;            // loglar icin zaman tutucu
static int INDEX = 0;                // okunan gorevlerin indeksini tutar

TaskQueue queue_u0 = {NULL, NULL, 0};     // 0 / fcfs
TaskQueue queue_u1 = {NULL, NULL, 0};     // 1 / user high
TaskQueue queue_u2 = {NULL, NULL, 0};     // 2 / user medium
TaskQueue queue_u3 = {NULL, NULL, 0};     // 3 / user low - round robin
TaskQueue killed_tasks = {NULL, NULL, 0}; // killed tasks

TaskParams* current_task = NULL; // mevcut yurutulmekte olan gorev

// terminal ciktisi icin rastgele renk uretir
void random_color(Color* color) {
    color->red = 56 + (rand() % 200);
    color->green = 56 + (rand() % 200);
    color->blue = 56 + (rand() % 200);
}

// timeout dolasiyla oldurulen gorevleri loglar
void log_timeouts() {
    TaskParams* iter = killed_tasks.head;
    while (iter) {
        printf(ANSI_COLOR_SET_24BIT_FG, iter->color.red, iter->color.green, iter->color.blue);
        printf("%07.4f sn   task%-2d   %-12s   │id:%-2d   oncelik:%-1d   kalan sure:%-2d sn", (float)CURR_TIME,
               iter->id + 1, "zamanasimi", iter->id, iter->priority, iter->remaining_time);
        printf(ANSI_COLOR_RESET);
        iter = iter->next;
    }
    killed_tasks.head = NULL;
    killed_tasks.tail = NULL;
    killed_tasks.count = 0;
}

// log mesajlarini ozellestirir
void logger_w_chars(const char* chars) {
    if (current_task == NULL) return;
    printf(ANSI_COLOR_SET_24BIT_FG, current_task->color.red, current_task->color.green, current_task->color.blue);
    printf("%07.4f sn   task%-2d   %-12s   │id:%-2d   oncelik:%-1d   kalan sure:%-2d sn", (float)CURR_TIME,
           current_task->id + 1, chars, current_task->id, current_task->priority, current_task->remaining_time);
    printf(ANSI_COLOR_RESET);
}

// arguman olarak gelen dosyadan gorevleri ice aktarir
TaskParams* parse_tasks_from_file(const char* f_name, int* task_count) {
    FILE* file = fopen(f_name, "r");
    if (file == NULL) {
        fprintf(stderr, "err parse_tasks_from_file() %s ?", f_name);
        *task_count = 0;
        return NULL;
    }

    TaskParams* tasks = NULL;
    int capacity = INITIAL_CAPACITY;
    int count = 0;
    char line[MAX_LINE_LENGTH];

    tasks = (TaskParams*)malloc(capacity * sizeof(TaskParams));
    if (tasks == NULL) {
        perror("mem alloc failed!");
        fclose(file);
        *task_count = 0;
        return NULL;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        int arrival_time, priority, cpu_time;
        if (sscanf(line, "%d, %d, %d", &arrival_time, &priority, &cpu_time) == 3) {
            if (count >= capacity) {
                capacity *= 2;
                TaskParams* temp = (TaskParams*)realloc(tasks, capacity * sizeof(TaskParams));
                if (temp == NULL) {
                    perror("mem realloc failed!");
                    free(tasks);
                    fclose(file);
                    *task_count = 0;
                    return NULL;
                }
                tasks = temp;
            }

            tasks[count].id = count;
            tasks[count].arrival_time = arrival_time;
            tasks[count].priority = priority;
            tasks[count].cpu_time = cpu_time;
            tasks[count].remaining_time = cpu_time;
            tasks[count].status = TASK_READY;

            tasks[count].next = NULL;
            tasks[count].prev = NULL;

            random_color(&tasks[count].color);

            count++;
        } else {
            fprintf(stderr, "warn could not parse line: %d. %s", count, line);
        }
    }

    fclose(file);
    *task_count = count;

    if (count > 0) {
        TaskParams* temp = (TaskParams*)realloc(tasks, count * sizeof(TaskParams));
        if (temp == NULL) {
            perror("final mem realloc failed!");
            return tasks;
        }
        tasks = temp;

        // FreeRTOS gorevi olustur
        for (int i = 0; i < count; i++) {
            // create the task
            BaseType_t result = xTaskCreate(generic_task,             // gorev fonksiyonu
                                            "WorkerTask",             // gorev ismi
                                            configMINIMAL_STACK_SIZE, // stack boyutu
                                            (void*)&tasks[i],         // gprev parametreleri (TaksParams)
                                            tskIDLE_PRIORITY + 1,     // oncelik
                                            &tasks[i].handle          // handler
            );

            if (result != pdPASS) {
                fprintf(stderr, "failed to create task %d\n", i);
            } else {
                vTaskSuspend(tasks[i].handle);
            }
        }

    } else {
        free(tasks);
        return NULL;
    }
    return tasks;
}

// timeout dolasiyla oldurulecek gorevleri temizler(kuyrugundan cikarir ve vTaskDelete() ile oldurur)
void kill_task(TaskQueue* queue, TaskParams* target) {
    if (target->next)
        target->next->prev = target->prev;
    else
        queue->tail = target->prev;
    if (target->prev)
        target->prev->next = target->next;
    else
        queue->head = target->next;
    queue->count--;

    // FreeRTOS icin de artik bu gorev yok
    vTaskDelete(target->handle);
    target->handle = NULL;
    target->status = TASK_FINISHED;

    enqueue(&killed_tasks, target);
}

// timeout olan gorevleri yakalar
void check_timeouts() {
    TaskQueue* queues[] = {&queue_u0, &queue_u1, &queue_u2, &queue_u3};

    for (int i = 0; i < 4; i++) {
        TaskQueue* curr_q = queues[i];
        TaskParams* iter = curr_q->head;
        TaskParams* next_task = NULL;
        while (iter) {
            next_task = iter->next;
            int alive_time = CURR_TIME - iter->arrival_time;
            if (alive_time >= 20) {
                kill_task(curr_q, iter);
            }
            iter = next_task;
        }
    }
}

// hedef kuyruga eleman ekler
void enqueue(TaskQueue* queue, TaskParams* task) {
    if (task == NULL) return;
    task->next = NULL;
    task->prev = NULL;
    if (queue->count == 0) {
        queue->head = task;
        queue->tail = task;
    } else {
        queue->tail->next = task;
        task->prev = queue->tail;
        queue->tail = task;
    }
    queue->count++;
}

// hedef kuyruktan siradaki elemani ceker
TaskParams* dequeue(TaskQueue* queue) {
    if (queue->count == 0 || queue->head == NULL) {
        return NULL;
    }
    TaskParams* task = queue->head;

    if (queue->count == 1) {
        queue->head = NULL;
        queue->tail = NULL;
    } else {
        queue->head = task->next;
        if (queue->head != NULL) {
            queue->head->prev = NULL;
        }
    }
    task->next = NULL;
    task->prev = NULL;

    queue->count--;
    return task;
}

// kuyruk secimi. bu bir gorevi oncelik dusurdukten sonra dogru kuyruga yonlendirir
void choose_enqueue(TaskParams* curr) {
    switch (curr->priority) {
    case 0:
        enqueue(&queue_u0, curr);
        break;
    case 1:
        enqueue(&queue_u1, curr);
        break;
    case 2:
        enqueue(&queue_u2, curr);
        break;
    case 3:
        enqueue(&queue_u3, curr);
        break;
    }
}

// her tick icin cagrilir ve gorevleri yonetir
void schedule_tick(void) {
    // yeni gelen gorev varsa uygun kuyruga ekle.
    for (int i = INDEX; i < TASK_COUNT; i++) {
        TaskParams* curr = &TASK_LIST[i];
        if (CURR_TIME == curr->arrival_time) {
            INDEX++;
            choose_enqueue(curr);
        } else {
            // daha varmamis olan ama varmasi en yakin olan gorev buna yakalanir. cunku index takibi ile
            // gecmiste varan gorevler tekrar kontrol edilmez
            break;
        }
    }

    // calisan gorev bittiyse sonlandir.
    if (current_task != NULL) {
        // kalan zamani bir azalt
        // eger gorev bittiysse siradakine gec
        if (--(current_task->remaining_time) == 0) {
            current_task->status = TASK_FINISHED;
            logger_w_chars("sonlandi");
            current_task = NULL;
        }
        // eger bitmediyse kontrol et
        else {
            // oncelik 0sa devam etsin.// alttaki gorev degistiriciye girmeyecek cunku null yapmadik.
            // oncelik 0 degilse oncelik azalt ve kuyrugunu degistir.
            if (current_task->priority != 0) {
                if (current_task->priority < 3) {
                    current_task->priority++;
                }
                choose_enqueue(current_task);
                current_task->status = TASK_READY;
                logger_w_chars("askida");
                current_task = NULL;
            } else {
                int alive_time = CURR_TIME - current_task->arrival_time;
                if (alive_time >= 20) {
                    enqueue(&queue_u0, current_task);
                    // current_task->status = TASK_READY; // birazdan direkt oldurulecek
                    current_task = NULL;
                } else
                    logger_w_chars("yurutuluyor");
            }
        }
    }

    check_timeouts();

    // calisan gorev suresi doldu mu (user task ise 1 sn calisir, sonra oncelik duser).
    if (current_task == NULL) {
        // eger buraya girildiyse demekki onceki gorev serbest birakilmis
        // queue_u0dan baslamak uzere en yakin gorevi aktif et
        if (queue_u0.count > 0) {
            current_task = dequeue(&queue_u0);
        } else if (queue_u1.count > 0) {
            current_task = dequeue(&queue_u1);
        } else if (queue_u2.count > 0) {
            current_task = dequeue(&queue_u2);
        } else if (queue_u3.count > 0) {
            current_task = dequeue(&queue_u3);
        }
        if (current_task) {
            current_task->status = TASK_RUNNING;
            if (current_task->cpu_time == current_task->remaining_time) {
                logger_w_chars("basladi");
            } else {
                logger_w_chars("yurutuluyor");
            }
        }
    }
    // logging
    log_timeouts();
    // printf("Tick...\n");
}

// schedulerin kendisi
void init_scheduler(TaskParams tasks[], int task_count) {
    TASK_LIST = tasks;
    TASK_COUNT = task_count;
    CURR_TIME = 0;
    INDEX = 0;
    srand(time(NULL));
}

void simulation_task(void* pvParameters) {
    printf("──────────────────────────────────────────────────────────────────────────────\n");

    TaskParams* prev_t = NULL;

    while (TASK_COUNT > 0 && !(TASK_COUNT == INDEX && queue_u0.count == 0 && queue_u1.count == 0 &&
                               queue_u2.count == 0 && queue_u3.count == 0 && current_task == NULL)) {

        prev_t = current_task;

        schedule_tick();

        if (prev_t != current_task) {
            if (prev_t != NULL) {
                if (prev_t->status == TASK_FINISHED) {
                    if (prev_t->handle != NULL) {
                        vTaskDelete(prev_t->handle);
                        prev_t->handle = NULL;
                    }
                } else {
                    if (prev_t->handle != NULL) {
                        vTaskSuspend(prev_t->handle);
                    }
                }
            }

            if (current_task != NULL && current_task->handle != NULL) {
                vTaskResume(current_task->handle);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
        CURR_TIME++;
    }
    printf("──────────────────────────────────────────────────────────────────────────────\n");
    vTaskEndScheduler();
}
