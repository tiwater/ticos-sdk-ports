#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void key_led_app_init(void);
void led_ctl(int status);

int get_led_light();
void set_led_light(int light);
int get_switch_state();


#ifdef __cplusplus
}
#endif