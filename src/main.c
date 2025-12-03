#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Scheduler'i baslatir ve FreeRTOS cekirdegini devreye sokar.
 */
int main(int argc, char** argv) {
    // argumanlari al
    if (argc != 2) {
        fprintf(stderr, "missing argument!\nplease use: ./freertos_sim <FILE_NAME>\n");
        return 1;
    }

    int count = 0; // dosyadan okunan gorev sayisi
    // parse_tasks_from_file fonksiyonu, gorevleri okur, TaskParams listesini olusturur
    // ve her TaskParams icin FreeRTOS worker task'lerini yaratip askiya alir
    TaskParams* list = parse_tasks_from_file(argv[1], &count);

    if (list == NULL || count == 0) {
        printf("there is no task here!");
        return 1;
    }

    // scheduler icin gerekli olan on hazirligi yapar
    init_scheduler(list, count);

    // Bu gorev, FreeRTOS cekirdegi icinde calisacak ve tum planlama mantigini yonetecek
    xTaskCreate(simulation_task,          // fonksiyon
                "Simulation",             // gorev adı
                configMINIMAL_STACK_SIZE, // yigin boyutu (FreeRTOSConfig.h'den gelir)
                NULL,                     // parametreler
                tskIDLE_PRIORITY + 2,     // oncelik
                NULL                      // task handler
    );

    // FreeRTOS Scheduler'ı başlatma
    vTaskStartScheduler();

    if (list != NULL) {
        free(list);
    }

    return 0;
}
