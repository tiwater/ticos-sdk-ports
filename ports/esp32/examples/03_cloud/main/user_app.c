#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "user_app.h"
#include "ticos_api.h"
#include "net_manage.h"
#include "esp_log.h"

#define TAG     "U_K_APP"

#define KEY_GPIO        1 //16
#define LED_GPIO        3 //7

#define KEY_SCAN_INTERVAL       20
#define KEY_CLICK               (60 / KEY_SCAN_INTERVAL)
#define KEY_LONG_PRESS          (1400 / KEY_SCAN_INTERVAL)

static int key_process = 0;
static int switch_state = 0;
static int led_light = 0;
static uint32_t key_tick = 0;

void led_ctl(int status)
{
    gpio_set_level(LED_GPIO, status);
}

int get_led_light()
{
    return led_light;
}

void set_led_light(int light)
{
    led_light = light;
    led_ctl(led_light);
}

int get_switch_state()
{
    return switch_state;
}

static int key_down()
{
    return !gpio_get_level(KEY_GPIO);
}

static void user_key_cb()
{
    // 更新按键状态
    if (switch_state)
        switch_state = 0;
    else
        switch_state = 1;

    // 翻转 LED 状态
    if (led_light) {
        led_light = 0;
    } else {
        led_light = 1;
    }
    led_ctl(led_light);
    ticos_property_report();
}

static void key_scan()
{
    if (key_down()) {
      // 响应按键按下动作
        if (!key_process) {
            if (++ key_tick >= KEY_LONG_PRESS) {
                key_process = 1;
                ESP_LOGI(TAG, "long press: reset net message");
                net_manage_reset();
            }
        }
    } else {
        // 抬起按键，清除状态
        if((!key_process) && (key_tick >= KEY_CLICK)){
            ESP_LOGI(TAG, "key click");
            user_key_cb();
        }
        key_process = 0;
        key_tick = 0;
    }
}

static void key_led_task(void *param)
{
    gpio_set_direction(KEY_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(KEY_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    set_led_light(0);

    while (1) {
        key_scan();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void key_led_app_init(void)
{
    TaskHandle_t taskHandle;
    xTaskCreate(key_led_task, "key_led_task", 4096, NULL, 4, &taskHandle);
}
