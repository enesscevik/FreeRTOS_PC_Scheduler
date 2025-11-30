#include "scheduler.h"
#include "time.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "missing argument!\nplease use: ./freertos_sim <FILE_NAME>\n");
        return 1;
    }
    printf("FreeRTOS_PC_Scheduler!\n");
    srand(time(NULL));

    int count = 0;
    TaskParams* list = parse_tasks_from_file(argv[1], &count);

    // for (int i = 0; i < count; i++) {
    //     printf("--------------------------\nid: %d\narrive: %d\npriority: %d\ncputime: %d\nremaining: %d\n",
    //     list[i].id,
    //            list[i].arrival_time, list[i].priority, list[i].cpu_time, list[i].remaining_time);
    // }

    scheduler(list, count);

    // free(list);
    return 0;
}
