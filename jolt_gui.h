#ifndef __JOLT_LVGL_GUI_H__
#define __JOLT_LVGL_GUI_H__

#include "../lvgl/lvgl.h"

void jolt_gui_create(lv_indev_t *kp_indev);

#define JOLT_GUI_BACK // Used to close window, or go to the left during entry
void jolt_gui_back_callback();

#define STATUSBAR_H 12

#ifndef CONFIG_JOLT_GUI_ANIM_DIGIT_MS
    #define CONFIG_JOLT_GUI_ANIM_DIGIT_MS 10
#endif

lv_group_t *g_back;

#endif
