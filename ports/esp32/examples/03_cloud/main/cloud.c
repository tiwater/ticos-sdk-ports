#include "hal_kv_db.h"
#include "shell.h"
#include "net_manage.h"
#include "user_app.h"

void cmd_init(void);
void thingmodel_init(void);

void app_main(void)
{
    hal_kv_init();
    net_manage_start();
    finsh_system_init();
    cmd_init();
    key_led_app_init();
    thingmodel_init();
}
