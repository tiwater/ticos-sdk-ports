#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define NAMESPACE       "nvs"

void hal_kv_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}

int hal_kv_set(const char *key, void *val, int len)
{
    nvs_handle_t handle;
    esp_err_t err;
    ESP_ERROR_CHECK(nvs_open(NAMESPACE, NVS_READWRITE, &handle));
    err = nvs_set_blob(handle, key, val, len);
    nvs_close(handle);
    return err;
}

int hal_kv_get(const char *key, void *buf, int len)
{
    nvs_handle_t handle;
    esp_err_t err;
    size_t required_size = 0;
    ESP_ERROR_CHECK(nvs_open(NAMESPACE, NVS_READWRITE, &handle));
    err = nvs_get_blob(handle, key, NULL, &required_size);
    if (err != ESP_OK) {
        ;
    } else if ((required_size == 0) || (required_size > len)) {
        err = ESP_ERR_NVS_NOT_ENOUGH_SPACE;
    } else {
        err = nvs_get_blob(handle, key, buf, &required_size);
    }
    nvs_close(handle);
    return err;
}

int hal_kv_del(const char *key)
{
    nvs_handle_t handle;
    esp_err_t err;
    ESP_ERROR_CHECK(nvs_open(NAMESPACE, NVS_READWRITE, &handle));
    err = nvs_erase_key(handle, key);
    nvs_close(handle);
    return err;
}

