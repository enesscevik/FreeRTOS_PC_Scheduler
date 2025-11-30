#ifndef SCHEDULER_H
#define SCHEDULER_H

// #include "FreeRTOS.h"
// #include "task.h"

typedef struct {
    int red;
    int green;
    int blue;
} Color;

typedef enum { TASK_READY, TASK_RUNNING, TASK_SUSPENDED, TASK_FINISHED } TaskStatus;

// giris.txt'den okunacak, gorev takibi icin kullanilacak
typedef struct TaskParams {
    int id;             // Gorev kimligi
    int arrival_time;   // gorev varis suresi /saniye
    int priority;       // gorev onceligi 0 realtime /1-2-3
    int cpu_time;       // toplam cpu suresi
    int remaining_time; // kalan cpu suresi
    TaskStatus status;  // gorev durumu
    Color color;        // Log ciktisi icin rastgele renk

    struct TaskParams* next;
    struct TaskParams* prev;

    // FreeRTOS entegrasyonu icin ilerde acilacak
    // TaskHandle_t handle;
} TaskParams;

// gorevlerin tutulacagi kuyruk yapisi
typedef struct {
    TaskParams* head;
    TaskParams* tail;
    int count;
} TaskQueue;

TaskParams* parse_tasks_from_file(const char*, int*);

void scheduler(TaskParams tasks[], int task_count);
void schedule_tick(void);

void enqueue(TaskQueue* queue, TaskParams* task);
TaskParams* dequeue(TaskQueue* queue);

// private //void random_color(Color* color);
// private //void log_timeouts();
// private //void logger_w_chars(const char*);

#endif // SCHEDULER_H
