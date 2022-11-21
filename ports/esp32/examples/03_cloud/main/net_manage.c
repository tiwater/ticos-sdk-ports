#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "hal_kv_db.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "shell.h"
#include "ticos_api.h"
#include "ble_qiot_export.h"

#define PRODUCT_ID      "BOB45WX7H4"
#define DEVICE_ID       "TEST002"
#define DEVICE_SECRET   "7rjQAIYU7DPULJo8YlppEg=="

static EventGroupHandle_t m_net_event;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define TICOS_CLOUD_CONNECT BIT2
#define WIFI_RESET          BIT3

static const char *TAG = "[net_manage]";
#define WIFI_CFG       "wifi_cfg"

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        ble_report_wificonn_state(BLE_WIFI_STATE_CONNECT);
        xEventGroupSetBits(m_net_event, WIFI_CONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *disconnected = (wifi_event_sta_disconnected_t*) event_data;
        ESP_LOGE(TAG, "Disconnect reason : %d", disconnected->reason);
        if (disconnected->reason == WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT) {
            ESP_LOGE(TAG, "wrong password");
        } else {
            ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
        }
        esp_wifi_connect();
        xEventGroupClearBits(m_net_event, WIFI_CONNECTED_BIT);
        xEventGroupSetBits(m_net_event, WIFI_FAIL_BIT);
    }
}

static void wifi_init_sta(void)
{
    m_net_event = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
}

static void ticos_cloud_evt_cb(void *user_data, ticos_evt_t evt)
{
    printf("%s:%d-evt:%d\n", __func__, __LINE__, evt);
    switch (evt) {
    case TICOS_EVENT_CONNECT:
        xEventGroupSetBits(m_net_event, TICOS_CLOUD_CONNECT);
        break;
    case TICOS_EVENT_DISCONNECT:
        xEventGroupClearBits(m_net_event, TICOS_CLOUD_CONNECT);
        break;
    }
}

static void net_manage_task(void *param)
{
    while (1) {
        ESP_LOGI(TAG, "net manage task run");
        int need_bind = 0;
        wifi_config_t cfg;
        int ret = hal_kv_get(WIFI_CFG, &cfg, sizeof(cfg));
        if (ret != ESP_OK) {
            ble_qiot_service_start();
            need_bind = 1;
        } else {
            esp_wifi_set_config(WIFI_IF_STA, &cfg);
        }
        esp_wifi_start();
        EventBits_t evts;
        evts = xEventGroupWaitBits(m_net_event, WIFI_CONNECTED_BIT | WIFI_RESET, 0, 0, -1);
        if (evts & WIFI_RESET)
            goto net_manage_end;

        ESP_LOGI(TAG, "net state ok!");
        set_ticos_event_cb(ticos_cloud_evt_cb, NULL);
        ticos_cloud_start(PRODUCT_ID, DEVICE_ID, DEVICE_SECRET);

        evts = xEventGroupWaitBits(m_net_event, TICOS_CLOUD_CONNECT | WIFI_RESET, 0, 0, -1);
        if (evts & WIFI_RESET)
            goto net_manage_end;

        if (need_bind) {
            char token[64];
            int len = sizeof(token);
            while (llsync_token_get(token, len) != ESP_OK)
                vTaskDelay(pdMS_TO_TICKS(500));
            esp_wifi_get_config(WIFI_IF_STA, &cfg);
            hal_kv_set(WIFI_CFG, &cfg, sizeof(cfg));
            ESP_LOGI(TAG, "cloud bind token: %s", token);
            llsync_token_clear();
            ble_qiot_service_stop();
        }

        xEventGroupWaitBits(m_net_event, WIFI_RESET, 1, 0, -1);

net_manage_end:
        hal_kv_del(WIFI_CFG);
        esp_wifi_restore();
        ticos_cloud_stop();
        xEventGroupClearBits(m_net_event, 0xff);
    }
}

void net_manage_start(void)
{
    wifi_init_sta();
    TaskHandle_t taskHandle;
    xTaskCreate(net_manage_task, "net_manage", 4096, NULL, 3, &taskHandle);
}

void net_manage_reset(void)
{
    xEventGroupSetBits(m_net_event, WIFI_RESET);
}

static int net_info(void)
{
    wifi_config_t cfg;
    int ret = hal_kv_get(WIFI_CFG, &cfg, sizeof(cfg));
    if (ret != ESP_OK) {
        printf("[net manage]: no wifi ssid save\n");
    } else {
        printf("[net manage]: wifi ssid: %s, passwd: %s\n", cfg.sta.ssid, cfg.sta.password);
    }

    esp_wifi_get_config(WIFI_IF_STA, &cfg);
    printf("[current set]: wifi ssid: %s, passwd: %s\n", cfg.sta.ssid, cfg.sta.password);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(net_info, netinfo, show wifi ssid and passwd.);
