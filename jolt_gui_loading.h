#ifndef __JOLT_GUI_LOADING_H__
#define __JOLT_GUI_LOADING_H__

#include "../../lvgl/lvgl.h"

#ifndef CONFIG_JOLT_GUI_LOADING_BAR_W
    #define CONFIG_JOLT_GUI_LOADING_BAR_W 100
#endif
#ifndef CONFIG_JOLT_GUI_LOADING_BAR_H
    #define CONFIG_JOLT_GUI_LOADING_BAR_H 15
#endif
#ifndef CONFIG_JOLT_GUI_LOADING_BAR_ANIM_MS
    #define CONFIG_JOLT_GUI_LOADING_BAR_ANIM_MS 300
#endif

#ifndef CONFIG_JOLT_GUI_LOADING_TEXT_W
    #define CONFIG_JOLT_GUI_LOADING_TEXT_W 100
#endif

lv_obj_t *jolt_gui_scr_loading_create(const char *title);
void jolt_gui_scr_loading_update(lv_obj_t *parent, uint8_t percentage, const char *text);


#endif
