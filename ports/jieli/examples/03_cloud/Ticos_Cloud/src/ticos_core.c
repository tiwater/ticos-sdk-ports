#include <stdio.h>
#include <string.h>

int ticos_cloud_start(char *product_id, char *device_name, char *psk)
{
    ticos_hal_ble_start(product_id, device_name, psk);
}

void ticos_cloud_stop()
{
    ticos_hal_ble_stop();
}

