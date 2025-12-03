#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "FreeRTOS.h"
#include "task.h"

/*
 * Renkli loglama icin renk yapisi.
 */
typedef struct {
    int red;
    int green;
    int blue;
} Color;

/*
 * Gorev Durumlari:
 * TASK_READY:    Gorev kuyrukta sirasini bekliyor (FreeRTOS: Suspended).
 * TASK_RUNNING:  Gorev su an islemci uzerinde (FreeRTOS: Running).
 * TASK_FINISHED: Gorev bitti veya zaman asimina ugradi (FreeRTOS: Deleted).
 */
typedef enum { TASK_READY, TASK_RUNNING, TASK_FINISHED } TaskStatus;

/*
 * Gorev Parametreleri
 * dosyadan okunan ve calisma zamaninda guncellenen veriler.
 */
typedef struct TaskParams {
    int id;             // Gorev Kimligi (0, 1, 2...)
    int arrival_time;   // Varis Zamani (saniye)
    int priority;       // Öncelik (0: Real-Time, 1-2-3: User Tasks)
    int cpu_time;       // Toplam Gereken CPU Suresi
    int remaining_time; // Kalan CPU Suresi

    TaskStatus status; // Gorevin Simülasyon Durumu
    Color color;       // Log ciktisi icin atanan rastgele renk

    // Kuyruk yönetimi icin cift yonlu bagli liste pointerlari
    struct TaskParams* next;
    struct TaskParams* prev;

    // FreeRTOS tarafindaki gercek gorev yoneticisi
    TaskHandle_t handle;
} TaskParams;

/*
 * Generic Task
 * Tüm simule edilen gorevler bu fonksiyonu calistirir.
 * Surekli CPU tuketimini simule eden bir donguye sahiptir.
 */
void generic_task(void* pvParameters);

/*
 * Gorev Kuyrugu
 * FIFO mantigiyla calisan cift yonlu bagli liste.
 */
typedef struct {
    TaskParams* head; // Listenin başı
    TaskParams* tail; // Listenin sonu
    int count;        // Eleman sayisi
} TaskQueue;

// Belirtilen dosyadan gorevleri okur, dinamik bellek ayirir ve listeyi dondurur.
// Hata durumunda NULL döner, task_count'u 0 yapar.
TaskParams* parse_tasks_from_file(const char* f_name, int* task_count);

// Scheduler icin gerekli globalleri ayarlar.
void init_scheduler(TaskParams tasks[], int task_count);

// Ana Simülasyon Gorevi
// FreeRTOS icinde calisir, diger gorevleri yonetir.
void simulation_task(void* pvParameters);

// Her simülasyon saniyesinde cagrilan planlayici mantigi.
void schedule_tick(void);

// Kuyrugun sonuna gorev ekler.
void enqueue(TaskQueue* queue, TaskParams* task);

// Kuyruktan siradaki gorevi cikarir ve dondurur.
TaskParams* dequeue(TaskQueue* queue);

// Loglama Yardimcilari
void logger_w_chars(const char* message);

#endif // SCHEDULER_H
