#ifndef __JOLT_LVGL_GUI_ENTRY_H__
#define __JOLT_LVGL_GUI_ENTRY_H__

#include <stddef.h>
#include "../lvgl/lvgl.h"
//#include "jolt_gui.h"

void jolt_gui_create(lv_indev_t *kp_indev);

#define JOLT_GUI_BACK // Used to close window, or go to the left during entry
void jolt_gui_back_callback();

//lv_res_t jolt_gui_pin_create( void ); 

/* Creates a numeric entry form with "n" digits and a decimal-point "decimal"
 * digits from the right. If decimal is negative, then no decimal will show.
 * Callback gets called after final roller entry.*/
#define JOLT_GUI_NO_DECIMAL -1
lv_obj_t *jolt_gui_numeric_create( int8_t n, int8_t decimal, const char *title,
        lv_action_t cb ); 


#ifndef CONFIG_JOLT_GUI_ANIM_DIGIT_MS
    #define CONFIG_JOLT_GUI_ANIM_DIGIT_MS 90
#endif
#ifndef CONFIG_JOLT_GUI_NUMERIC_LEN
    // Max number of rollers
    #define CONFIG_JOLT_GUI_NUMERIC_LEN 8
#endif
#ifndef CONFIG_JOLT_GUI_PIN_LEN
    // note: must be less than or equal to CONFIG_JOLT_GUI_NUMERIC_LEN
    #define CONFIG_JOLT_GUI_PIN_LEN 8
#endif

#endif
