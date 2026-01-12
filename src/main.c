#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Starts the scheduler and initializes the FreeRTOS kernel. 
 */
int main(int argc, char** argv) {
    // get arguments
    if (argc != 2) {
        fprintf(stderr, "missing argument!\nplease use: ./freertos_sim <FILE_NAME>\n");
        return 1;
    }

    int count = 0; // number of tasks read from file
    // parse_tasks_from_file reads tasks, creates TaskParams list, and creates/suspends
    // FreeRTOS worker tasks for each.
    TaskParams* list = parse_tasks_from_file(argv[1], &count);

    if (list == NULL || count == 0) {
        printf("there is no task here!");
        return 1;
    }

    // Performs preliminary preparation for the scheduler.
    init_scheduler(list, count);

    // This task will run within the FreeRTOS kernel and manage all scheduling logic.
    xTaskCreate(simulation_task,          // function
                "Simulation",             // task name
                configMINIMAL_STACK_SIZE, // stack size (from FreeRTOSConfig.h)
                NULL,                     // parameters
                tskIDLE_PRIORITY + 2,     // priority
                NULL                      // task handler
    );

    // Start FreeRTOS scheduler
    vTaskStartScheduler();

    if (list != NULL) {
        free(list);
    }

    return 0;
}
