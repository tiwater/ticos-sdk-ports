#include "system/app_core.h"
#include "system/includes.h"
#include "server/server_core.h"
#include "app_config.h"
#include "app_action.h"
#include "os/os_api.h"
#include "btcontroller_config.h"
#include "btctrler/btctrler_task.h"
#include "config/config_transport.h"
#include "btstack/avctp_user.h"
#include "btstack/btstack_task.h"
#include "bt_common.h"
#include "rcsp_bluetooth.h"
#include "rcsp_user_update.h"
#include "app_charge.h"
#include "app_chargestore.h"
#include "app_power_manage.h"
#include "app_comm_bt.h"

#define LOG_TAG             "[TICOS]"
#include "debug.h"

#if !(TCFG_USER_BLE_ENABLE && (!TCFG_USER_EDR_ENABLE))
#error "board config error, confirm!!!!!!"
#endif

static u8 is_active = 0;

static void ble_pll_init(void)
{
    clk_set("sys", BT_NORMAL_HZ);
    u32 sys_clk =  clk_get("sys");
    bt_pll_para(TCFG_CLOCK_OSC_HZ, sys_clk, 0, 0);
}

void ticos_hal_ble_start()
{
    if (is_active){
        ble_qiot_advertising_start();
        return;
    }else{
        ble_pll_init();
    }
    is_active = 1;
    btstack_ble_start_before_init(NULL, 0);
    btstack_init();
    ble_qiot_advertising_start();
}

void ticos_hal_ble_stop()
{
    ble_disconnect();
}

static int ticos_bt_hci_event_handler(struct bt_event *bt)
{
    //对应原来的蓝牙连接上断开处理函数  ,bt->value=reason
    log_info("----%s reason %x %x", __FUNCTION__, bt->event, bt->value);
    bt_comm_ble_hci_event_handler(bt);
    return 0;
}

static int ticos_bt_connction_status_event_handler(struct bt_event *bt)
{
    log_info("----%s %d", __FUNCTION__, bt->event);

    bt_comm_ble_status_event_handler(bt);
    return 0;
}

static int ticos_event_handler(struct sys_event *event)
{
    log_info("----%s %d", __FUNCTION__, event->type);

    switch (event->type) {
    case SYS_BT_EVENT:
        if ((u32)event->arg == SYS_BT_EVENT_TYPE_CON_STATUS) {
            ticos_bt_connction_status_event_handler(&event->u.bt);
        } else if ((u32)event->arg == SYS_BT_EVENT_TYPE_HCI_STATUS) {
            ticos_bt_hci_event_handler(&event->u.bt);
        }
        return 0;

    default:
        return 0;
    }
    return 0;
}

SYS_EVENT_HANDLER(SYS_BT_EVENT, ticos_event_handler, 0);


