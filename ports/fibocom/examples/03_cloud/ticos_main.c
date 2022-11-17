#include "fibo_opencpu.h"
#include <ticos_api.h>

#define PRODUCT_ID  "BOB45WX7H4"
#define DEVICE_ID   "TEST002"
#define DEVICE_SECRET   "7rjQAIYU7DPULJo8YlppEg=="


extern void user_signal_process(GAPP_SIGNAL_ID_T sig, va_list arg);
FIBO_CALLBACK_T user_callback = {
    .fibo_signal = user_signal_process};

void *appimg_enter(void *param)
{
    OSI_LOGI(0, "application image enter, param 0x%x", param);
    ticos_cloud_start(PRODUCT_ID, DEVICE_ID, DEVICE_SECRET);
    return &user_callback;
}

void appimg_exit(void)
{
    ticos_cloud_stop();
    OSI_LOGI(0, "application image exit");
}
