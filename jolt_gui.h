#ifndef __JOLT_LVGL_GUI_H__
#define __JOLT_LVGL_GUI_H__

#include "../lvgl/lvgl.h"
#include "jolt_gui_entry.h"

/**********************
 *   GLOBAL VARIABLES
 **********************/
struct {
    struct {
        lv_group_t *main; // Parent group for user input
        lv_group_t *back; // Group used to handle back button
    } group;
    struct {
        lv_obj_t *rollers[CONFIG_JOLT_GUI_PIN_LEN];
        int8_t pos; // Dictates function of back button
        uint8_t spacing;
    } digit;
} jolt_gui_store;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/* Creates the Jolt GUI */
void jolt_gui_create(lv_indev_t *kp_indev);

/* Creates a title in the top left statusbar. 
 * Allocates and copies the title string. */
lv_obj_t *jolt_gui_title_create(lv_obj_t *parent, const char *title);

/* Creates a submenu of correct size, location, group, and infocus. 
 * Need to provide arguments for first element so focus works correctly.*/
lv_obj_t *jolt_gui_menu_create(const char *title, const void *img_src, 
        const char *txt, lv_action_t rel_action);

/* Creates a parent object for a new screen thats easy to delete */
lv_obj_t *jolt_gui_parent_create();

/* Deletes the container of the currently focused object */
bool jolt_gui_delete_current_screen();

#ifndef CONFIG_JOLT_GUI_STATUSBAR_H
    #define CONFIG_JOLT_GUI_STATUSBAR_H 12
#endif

#endif
