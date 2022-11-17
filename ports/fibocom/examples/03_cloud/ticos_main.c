#include "fibo_opencpu.h"
#include <ticos_api.h>

#define PRODUCT_ID  "BOB45WX7H4"
#define DEVICE_ID   "TEST003"
#define DEVICE_SECRET   "IsL431Z6gjKPn03cpdN7kA=="


extern void user_signal_process(GAPP_SIGNAL_ID_T sig, va_list arg);
static void prvInvokeGlobalCtors(void)
{
    extern void (*__init_array_start[])();
    extern void (*__init_array_end[])();

    size_t count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; ++i)
        __init_array_start[i]();
}

static void prvThreadEntry(void *param)
{
    OSI_LOGI(0, "application thread enter, param 0x%x", param);
    //srand(100);

    for (; ; )
    {
        OSI_LOGI(0, "report property ");
        ticos_property_report();
        fibo_taskSleep(3000);
    }
    fibo_thread_delete();
}

FIBO_CALLBACK_T user_callback = {
    .fibo_signal = user_signal_process};

void *appimg_enter(void *param)
{
    OSI_LOGI(0, "application image enter, param 0x%x", param);
    prvInvokeGlobalCtors();
    fibo_thread_create(prvThreadEntry, "mythread", 1024*4, NULL, OSI_PRIORITY_NORMAL);
    ticos_cloud_start(PRODUCT_ID, DEVICE_ID, DEVICE_SECRET);
    return &user_callback;
}

void appimg_exit(void)
{
    ticos_cloud_stop();
    OSI_LOGI(0, "application image exit");
}
