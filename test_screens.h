#ifndef __JOLT_GUI_TEST_SCREENS_H__
#define __JOLT_GUI_TEST_SCREENS_H__

#if PC_SIMULATOR
#include "../lvgl.h"

/* Displays text on the screen jolt_gui_text_create().
 * Tests for word wrapping and scrolling. */
lv_res_t jolt_gui_test_text_create(lv_obj_t * list_btn);

/* View characters of font */
lv_res_t jolt_gui_test_alphabet_create(lv_obj_t * list_btn);

/* Tests Submenuing system */
lv_res_t jolt_gui_test_submenu_create(lv_obj_t * list_btn);

/* Test Loading Animations on a task */
lv_res_t jolt_gui_test_loading_create(lv_obj_t *list_btn);

#endif // PC_SIMULATOR
#endif // Guard
