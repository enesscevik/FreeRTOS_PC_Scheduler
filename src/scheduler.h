#ifndef SCHEDULER_H
#define SCHEDULER_H

// #include "FreeRTOS.h"
// #include "task.h"

typedef enum { TASK_READY, TASK_RUNNING, TASK_SUSPENDED, TASK_FINISHED } TaskStatus;

// giris.txt'den okunacak, gorev takibi icin kullanilacak
typedef struct {
    int id;             // Gorev kimligi
    int arrival_time;   // gorev varis suresi /saniye
    int priority;       // gorev onceligi 0 realtime /1-2-3
    int cpu_time;       // toplam cpu suresi
    int remaining_time; // kalan cpu suresi
    TaskStatus status;  // gorev durumu
    //    TaskHandle_t handle; // FreeRTOS handler
} TaskParams;

TaskParams* parse_tasks_from_file(const char*, int*);

/**
 * @brief scheduler baslaticisi
 *
 * @param gorevleri tutan dizi
 * @param dizideki toplam gorev sayisi
 */
void start_scheduler(TaskParams tasks[], int task_count);

#endif // SCHEDULER_H
