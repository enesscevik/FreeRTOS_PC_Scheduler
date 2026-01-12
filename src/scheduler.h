#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "FreeRTOS.h"
#include "task.h"

/*
 * Color structure for colored logging.
 */
typedef struct {
    int red;
    int green;
    int blue;
} Color;

/*
 * Task Statuses:
 * TASK_READY:    Task is waiting in queue (FreeRTOS: Suspended).
 * TASK_RUNNING:  Task is currently on the processor (FreeRTOS: Running).
 * TASK_FINISHED: Task finished or timed out (FreeRTOS: Deleted).
 */
typedef enum { TASK_READY, TASK_RUNNING, TASK_FINISHED } TaskStatus;

/*
 * Task Parameters
 * Data read from file and updated at runtime.
 */
typedef struct TaskParams {
    int id;             // Task ID (0, 1, 2...)
    int arrival_time;   // Arrival Time (seconds)
    int priority;       // Priority (0: Real-Time, 1-2-3: User Tasks)
    int cpu_time;       // Total Required CPU Time
    int remaining_time; // Remaining CPU Time
    int last_status_change; // Status change time

    TaskStatus status; // Task Simulation Status
    Color color;       // Random color assigned for log output

    // Doubly linked list pointers for queue management
    struct TaskParams* next;
    struct TaskParams* prev;

    // Real task manager on FreeRTOS side
    TaskHandle_t handle;
} TaskParams;

/*
 * Generic Task
 * All simulated tasks run this function.
 * It has a loop that simulates continuous CPU consumption.
 */
void generic_task(void* pvParameters);

/*
 * Task Queue
 * Doubly linked list working with FIFO logic.
 */
typedef struct {
    TaskParams* head; // Head of list
    TaskParams* tail; // Tail of list
    int count;        // Number of elements
} TaskQueue;

// Reads tasks from the specified file, allocates dynamic memory, and returns the list.
// Returns NULL on error, sets task_count to 0.
TaskParams* parse_tasks_from_file(const char* f_name, int* task_count);

// Sets necessary globals for the Scheduler.
void init_scheduler(TaskParams tasks[], int task_count);

// Main Simulation Task
// Runs inside FreeRTOS, manages other tasks.
void simulation_task(void* pvParameters);

// Scheduler logic called every simulation second.
void schedule_tick(void);

// Adds task to the end of the queue.
void enqueue(TaskQueue* queue, TaskParams* task);

// Dequeues the next task from the queue and returns it.
TaskParams* dequeue(TaskQueue* queue);

// Logging Helpers
void logger_w_chars(const char* message);

#endif // SCHEDULER_H