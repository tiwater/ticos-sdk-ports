#include "shell.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_system.h"

void cmd_init(void)
{
}

int cmd_heap_dump(void)
{
    heap_caps_dump_all();
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_heap_dump, heap_dump, dump the memory in the system.);

int cmd_heap_info(void)
{
    printf("free heap size: %d, internal size: %d, minimum size: %d\n",
            esp_get_free_heap_size(),
            esp_get_free_internal_heap_size(),
            esp_get_minimum_free_heap_size());
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_heap_info, heap_info, Show the memory usage in the system.);

int cmd_ps(void)
{
    const int max_task_num = 50;
    uint32_t total_run_time;
    TaskStatus_t *tasks = (TaskStatus_t *)malloc(sizeof(TaskStatus_t) * max_task_num);
    int task_cnt = uxTaskGetSystemState(tasks, max_task_num, &total_run_time);
    printf("task info: totals:%d\n", task_cnt);
    for (int i = 0; i < task_cnt; i++) {
        printf("[%d.%s:%p], [stack:%p.%d], state:%d, time:%d, pro:%d.%d\n",
        tasks[i].xTaskNumber, tasks[i].pcTaskName, tasks[i].xHandle,
        tasks[i].pxStackBase, tasks[i].usStackHighWaterMark,
        tasks[i].eCurrentState, tasks[i].ulRunTimeCounter,
        tasks[i].uxCurrentPriority, tasks[i].uxBasePriority);
    }
    free(tasks);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_ps, ps, List threads in the system.);
