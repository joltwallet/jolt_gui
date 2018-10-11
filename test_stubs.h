#if PC_SIMULATOR
#ifndef __JOLT_GUI_TEST_STUBS_H__
#define __JOLT_GUI_TEST_STUBS_H__
#include "../lvgl/lvgl.h"

uint8_t get_battery_level();
uint8_t get_bluetooth_level();
uint8_t get_wifi_level();
uint8_t get_lock_status();

lv_res_t list_release_action(lv_obj_t * list_btn);

#endif
#endif
