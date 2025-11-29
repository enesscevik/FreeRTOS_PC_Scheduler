#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define INITIAL_CAPACITY 26

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
    } else {
        free(tasks);
        return NULL;
    }
    return tasks;
}

void start_scheduler(TaskParams tasks[], int task_count) {
    (void)tasks;      // for suppress
    (void)task_count; // for suppress
    printf("Scheduler started (placeholder).\n");
}
