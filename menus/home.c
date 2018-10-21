#include "jolt_gui/jolt_gui.h"
#include "jolt_gui/lv_theme_jolt.h"
#include "settings.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/


static lv_action_t woof(lv_obj_t btn) {
    printf("woof\n");
    return 0;
}

static lv_action_t back_release_action(lv_obj_t *btn) {
    jolt_gui_delete_current_screen();
    return 0;
}

void jolt_gui_menu_home_create() {
    /* Set Jolt ssd1306 theme */
    lv_theme_t *th = lv_theme_jolt_init(100, &synchronizer7);
    lv_theme_set_current(th);  

    // Don't need to set group since indev driver sends direct keypresses
    lv_obj_t *btn_back = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, &back_release_action);
    lv_group_add_obj(jolt_gui_store.group.back, btn_back);

    /* Create StatusBar */
    statusbar_create();

    /*Create the list*/
#if PC_SIMULATOR
    jolt_gui_store.main_menu_list = jolt_gui_menu_create("Main", NULL, "PIN Entry",
            jolt_gui_test_pin_create);
    lv_obj_t *mmlist = jolt_gui_store.main_menu_list;
    lv_list_add(mmlist, NULL, "Loading Test", jolt_gui_test_loading_create);
    lv_list_add(mmlist, NULL, "Alphabet", jolt_gui_test_alphabet_create);
    lv_list_add(mmlist, NULL, "Numeric Begin", jolt_gui_test_numeric_begin_dp_create);
    lv_list_add(mmlist, NULL, "Numeric End", jolt_gui_test_numeric_end_dp_create);
    lv_list_add(mmlist, NULL, "Numeric Mid", jolt_gui_test_numeric_mid_dp_create);
    lv_list_add(mmlist, NULL, "Text Test", jolt_gui_test_text_create);
    lv_list_add(mmlist, NULL, "Submenu", jolt_gui_test_submenu_create);
#elif ESP_PLATFORM
    if( true && jolt_gui_store.first_boot ) {
        jolt_gui_first_boot_create();
    }
    else {
        jolt_gui_store.main_menu_list = jolt_gui_menu_create("Main", NULL, "PIN Entry", woof);
        lv_obj_t *mmlist = jolt_gui_store.main_menu_list;
        lv_list_add(mmlist, NULL, "Settings", menu_settings_create);
        lv_list_add(mmlist, NULL, "woof", woof);
        lv_list_add(mmlist, NULL, "Dummy 2", NULL);
        lv_list_add(mmlist, NULL, "Dummy 3", NULL);
        lv_list_add(mmlist, NULL, "Dummy 4", NULL);
        lv_list_add(mmlist, NULL, "Dummy 5", NULL);
        lv_list_add(mmlist, NULL, "Dummy 6", NULL);
        lv_list_add(mmlist, NULL, "Dummy 7", NULL);
    }
#endif
}

