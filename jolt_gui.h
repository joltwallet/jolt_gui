#ifndef __JOLT_LVGL_GUI_H__
#define __JOLT_LVGL_GUI_H__

#include "lvgl.h"
#include "lv_conf.h"
#include "jolt_gui_entry.h"
#include "jolt_gui_statusbar.h"
#include "jolt_gui_symbols.h"
#include "jolt_gui_first_boot.h"
#include "jolt_gui_qr.h"
#include "menus/home.h"

#if PC_SIMULATOR
    #include "test_stubs.h"
    #include "test_screens.h"
#elif ESP_PLATFORM
    #include "freertos/FreeRTOS.h"
    #include "freertos/queue.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"
    #include "hal/hw_monitor.h"
#endif


#ifndef CONFIG_JOLT_GUI_LOADING_BUF_SIZE
    #define CONFIG_JOLT_GUI_LOADING_BUF_SIZE 30
#endif
/**********************
 *   GLOBAL VARIABLES
 **********************/

struct {
    bool first_boot;
    SemaphoreHandle_t mutex; // mutex for the entire gui system
    lv_obj_t *main_menu_list;
    struct {
        lv_group_t *main; // Parent group for user input
        lv_group_t *back; // Group used to handle back button
        lv_group_t *enter;
    } group;
    struct {
        lv_obj_t *container;
        lv_obj_t *label;
        hardware_monitor_t indicators[JOLT_GUI_STATUSBAR_INDEX_NUM];
    } statusbar;
    struct {
        lv_obj_t *bar; 
        lv_obj_t *label;
    } loading; // only 1 loading bar will ever be displayed at a time
} jolt_gui_store;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/* Run before running jolt_gui_create();
 * Creates all the groups and registers the in-device to the groups */
void jolt_gui_group_create();

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

/* Display Text; Pressing any button returns to previous screen */
lv_obj_t *jolt_gui_text_create(const char *title, const char *body);

/* Calls cb whenever the back button is pressed */
lv_obj_t *jolt_gui_set_back_action(lv_obj_t *parent, lv_action_t cb);
lv_obj_t *jolt_gui_set_enter_action(lv_obj_t *parent, lv_action_t cb);

lv_action_t jolt_gui_send_enter_main(lv_obj_t *btn);
lv_action_t jolt_gui_send_left_main(lv_obj_t *btn);

#endif
