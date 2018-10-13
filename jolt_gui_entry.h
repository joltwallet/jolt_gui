#ifndef __JOLT_LVGL_GUI_ENTRY_H__
#define __JOLT_LVGL_GUI_ENTRY_H__

#include <stddef.h>
#include "../lvgl/lvgl.h"
//#include "jolt_gui.h"

void jolt_gui_create(lv_indev_t *kp_indev);

#define JOLT_GUI_BACK // Used to close window, or go to the left during entry
void jolt_gui_back_callback();

lv_res_t jolt_gui_pin_create( void ); 

#ifndef CONFIG_JOLT_GUI_ANIM_DIGIT_MS
    #define CONFIG_JOLT_GUI_ANIM_DIGIT_MS 90
#endif
#ifndef CONFIG_JOLT_GUI_PIN_LEN
    #define CONFIG_JOLT_GUI_PIN_LEN 8
#endif


#endif
