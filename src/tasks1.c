#include "scheduler.h"

/*
 * FreeRTOS Worker Task function.
 * This function represents each work task managed by the simulation.
 * It consumes processor time when started by the scheduler
 * and remains in wait state when stopped.
 *
 * pvParameters: Parameter passed to the task by FreeRTOS.
 * In this simulation, it is of type TaskParams* but is not used directly
 * because processor consumption independent of the task is simulated.
 */
void generic_task(void* pvParameters) {
    // pvParameters: Actually, we receive a parameter of type TaskParams*.
    // However, since these worker tasks only simulate processor consumption,
    // they do not need to use their own IDs or other information.
    // All logging and state management is done by the Scheduler (simulation_task).
    // TaskParams* params = (TaskParams*)pvParameters; // Could be used like this if needed.

    for (;;) {
        // This infinite loop simulates the task continuously using processor resources.
        // The loop continues to run and occupy the processor until the FreeRTOS Scheduler (simulation_task)
        // stops this task with 'vTaskSuspend()'.
    }
}