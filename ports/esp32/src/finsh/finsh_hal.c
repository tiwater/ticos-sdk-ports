#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void *hal_thread_create(const char *name,
                        void (*task_func)(void*),
                        void *param, 
                        int stack_size,
                        int priority)
{
    TaskHandle_t task_handle;
    xTaskCreate(task_func, name, stack_size, param, priority, &task_handle);
    return task_handle;
}

#if CONFIG_IDF_TARGET_ESP32S3

#include <stdio.h>
#include <unistd.h>

char rt_hw_console_getchar(void)
{
    char ch;
    if(read(STDIN_FILENO, &ch, 1) > 0) {
        return ch;
    }
    usleep(20 * 1000);
    return -1;
}

#else
#include "driver/uart.h"
char rt_hw_console_getchar(void)
{
    static int u_init = 0;
    if (!u_init) {
        u_init = 1;
        uart_driver_install(UART_NUM_0, 128 * 2, 0, 0, NULL, 0);
    }

    char data;
    int len = uart_read_bytes(UART_NUM_0, &data, 1, -1);
    assert(len == 1);
    return data;
}
#endif