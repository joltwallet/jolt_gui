#include "jolt_gui.h"
#include "stdio.h"
//#include "lvgl/lv_themes/lv_theme.h"
#include "lv_theme_jolt.h"

#define MSG(...) printf(__VA_ARGS__)
/*********************
 *      DEFINES
 *********************/
#define N_PIN_DIGITS 6
#define PIN_SPACING 2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
lv_obj_t *jolt_gui_title_create(lv_obj_t *parent, const char *title);
static lv_res_t jolt_gui_settings_create(lv_obj_t * list_btn);
static lv_obj_t *menu_create(const char *title, const void *img_src, 
        const char *txt, lv_action_t rel_action);
static lv_res_t jolt_gui_pin_create(); 
static lv_obj_t *parent_create();

/* Stubs */
static lv_res_t list_release_action(lv_obj_t * list_btn);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_group_t *g_main; // Parent group for user input
lv_group_t *g_pin = NULL;
lv_style_t header_style;

static lv_obj_t *statusbar_container;

static lv_obj_t *main_menu_list;

static int8_t numerical_entry_loc = -1; // Dictates function of back button
static lv_obj_t *rollers[N_PIN_DIGITS];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static lv_action_t back_release_action(lv_obj_t *btn) {
    uint8_t id = lv_obj_get_free_num(btn);

    lv_obj_t *submenu = lv_group_get_focused(g_main);
    if( NULL == submenu ) {
        MSG("Nothing in focus\n");
        return 0;
    }
    if( submenu == main_menu_list ) {
        MSG("Can't exit main menu\n");
        return 0;
    }

    if( numerical_entry_loc <= 0 ) {
        // Close the current container
        lv_group_set_focus_cb(g_main, NULL); // Disable any focus callback
        numerical_entry_loc = -1; // just to be doubly sure
        lv_obj_t *parent = submenu;
        lv_obj_t *tmp = submenu;
        while( tmp = lv_obj_get_parent(tmp) ) {
            if( tmp != lv_scr_act() ) {
                parent = tmp;
            }
            if( tmp == main_menu_list ) {
                return 0;
            }
        }
        MSG("deleting %p\n", parent);
        lv_obj_del(parent);
    }
    else {
        // Preserve the currently selected value
        lv_roller_ext_t *ext = lv_obj_get_ext_attr(rollers[numerical_entry_loc]);
        ext->ddlist.sel_opt_id_ori = ext->ddlist.sel_opt_id;

        // Decrement position and refocus
        numerical_entry_loc--;
        MSG("Decrementing numerical_entry_loc to %d\n", numerical_entry_loc);
        lv_group_focus_obj(rollers[numerical_entry_loc]);
    }
    return 0;
}

static lv_group_focus_cb_t update_selected_roller_style(lv_obj_t *roller) {
    /* Only the focused roller has 3 visible selections;
     * all unselected rollers only have 1 */
    MSG("roller style callback!\n");
    if(numerical_entry_loc != 0 ) {
	    lv_obj_align(rollers[0], NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
    }
    for(uint8_t i=0; i<N_PIN_DIGITS; i++){
        uint8_t n_visible = 1;
        if( i == numerical_entry_loc ) {
            n_visible = 3;
        }
        lv_roller_set_visible_row_count(rollers[i], n_visible);
        if( i > 0 ) {
	        lv_obj_align(rollers[i], rollers[i-1], LV_ALIGN_OUT_RIGHT_MID, 
                    PIN_SPACING, 0);
        }
        else {
	        lv_obj_align(rollers[i], NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
        }
    }
    return 0;
}

static lv_action_t digit_entry_cb(lv_obj_t *roller) {
    /* Callback used for all digit roller except the final */
    numerical_entry_loc++;
    MSG("Incrementing numerical_entry_loc to %d\n", numerical_entry_loc);
    lv_group_send_data(g_main, LV_GROUP_KEY_NEXT);
    lv_group_focus_obj(rollers[numerical_entry_loc]);
    return 0;
}

static lv_action_t pin_entry_final_cb(lv_obj_t roller) {
    // todo: compute pin hash
    // todo: delete container
    return 0;
}

static lv_res_t jolt_gui_pin_create() { 
    lv_obj_t *parent = parent_create();

    lv_obj_t *pin_container = lv_cont_create(parent, NULL);
    lv_obj_set_size(pin_container, LV_HOR_RES, LV_VER_RES - STATUSBAR_H);
	lv_obj_align(pin_container, NULL, 
            LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_H);

    rollers[0] = lv_roller_create(pin_container, NULL);
	lv_roller_set_visible_row_count(rollers[0], 3);
    lv_roller_set_anim_time(rollers[0], CONFIG_JOLT_GUI_ANIM_DIGIT_MS);
	lv_obj_align(rollers[0], NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
	lv_roller_set_options(rollers[0], 
            "0\n1\n2\n3\n4\n5\n6\n7\n8\n9"
            "\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9"
            "\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9"
            );
    lv_roller_set_selected(rollers[0], 10, false);
    lv_roller_set_action(rollers[0], digit_entry_cb);
    lv_group_add_obj(g_main, rollers[0]);

    for( uint8_t i=1; i < N_PIN_DIGITS; i++ ) {
        rollers[i] = lv_roller_create(pin_container, rollers[0]);
	    lv_roller_set_visible_row_count(rollers[i], 1);
	    lv_obj_align(rollers[i], rollers[i-1], LV_ALIGN_OUT_RIGHT_MID, 
                PIN_SPACING, 0);
    }
    lv_roller_set_action(rollers[N_PIN_DIGITS-1], pin_entry_final_cb);

    lv_group_add_obj(g_main, pin_container);
    lv_group_focus_obj(rollers[0]);
    numerical_entry_loc = 0;
    
    lv_group_set_focus_cb(g_main, &update_selected_roller_style);

    jolt_gui_title_create(parent, "PIN (1/10)");
    MSG("PIN Container %p\n", pin_container);

    return LV_RES_OK;
}

static lv_res_t list_release_action(lv_obj_t * list_btn) {
    /* PLACEHOLDER STUB*/
    MSG("List element click:%s\n", lv_list_get_btn_text(list_btn));
    return LV_RES_OK; /*Return OK because the list is not deleted*/
}

static lv_obj_t *parent_create() {
    lv_obj_t *parent = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(parent, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(parent,0,0);
    lv_obj_set_style(parent, &lv_style_transp);
    return parent;
}

static lv_obj_t *menu_create(const char *title, const void *img_src, 
        const char *txt, lv_action_t rel_action) {
    /* Creates a submenu of correct size, location, group, and infocus. 
     * Title String must exist while menu exists.
     *
     * Need to provide arguments for first element so focus works correctly.*/

    /* Create common parent object to ease deletion.
     * Must be full size of screen to show Title */
    lv_obj_t *parent = parent_create();

    /* Create List */
    lv_obj_t *menu = lv_list_create(parent, NULL);
	lv_obj_set_size(menu, LV_HOR_RES, LV_VER_RES - STATUSBAR_H);
	lv_obj_align(menu, NULL, 
            LV_ALIGN_IN_TOP_LEFT, 0, STATUSBAR_H);
    lv_list_add(menu, img_src, txt, rel_action);
    lv_group_add_obj(g_main, menu);
    lv_group_focus_obj(menu);


    /* Create and Stylize Statusbar Title */
    jolt_gui_title_create(parent, title);
    return menu;
}

lv_obj_t *jolt_gui_title_create(lv_obj_t *parent, const char *title) {
    /* Creates a title in the top left statusbar. 
     * Allocates and copies the title string. */
    if( NULL == parent ) {
        parent = lv_scr_act();
    }
    lv_obj_t *label = lv_label_create(parent, NULL);

    static lv_style_t label_style;
    lv_style_copy(&label_style, &lv_style_plain);
    //lv_style_copy(&label_style, lv_obj_get_style(parent));
    label_style.body.main_color = LV_COLOR_WHITE;
    label_style.body.padding.ver = 1;

    lv_label_set_long_mode(label, LV_LABEL_LONG_ROLL);
    lv_label_set_body_draw(label, true); // draw background
    lv_label_set_style(label, &label_style);
    lv_obj_align(label, statusbar_container, LV_ALIGN_IN_LEFT_MID, 2, 0);
    lv_label_set_text(label, title);
    lv_obj_set_size(label, 80, 8);

    return label;
}

static lv_res_t jolt_gui_settings_create(lv_obj_t * list_btn) {
	/*Create the list*/
    lv_obj_t *settings_list = menu_create("Settings",
            NULL, "WiFi", list_release_action);
	lv_list_add(settings_list, NULL, "Bluetooth", list_release_action);
	lv_list_add(settings_list, NULL, "Factory Reset", list_release_action);
    MSG("settings_list %p\n", settings_list);
    return LV_RES_OK;
}

static void statusbar_create() {
    /* Create StatusBar Container */
    statusbar_container = lv_cont_create(lv_scr_act(), NULL);
    lv_style_copy(&header_style, lv_cont_get_style(statusbar_container) );
    header_style.body.border.part = LV_BORDER_BOTTOM;
    header_style.body.border.color = LV_COLOR_BLACK;

    lv_cont_set_style(statusbar_container, &header_style);
	lv_obj_set_size(statusbar_container, LV_HOR_RES, STATUSBAR_H);

    /* TODO: create statusbar update stuff via lv_task_create() */
    //menu_label = lv_label_create(statusbar_container, NULL);
    //lv_obj_t * img1 = lv_img_create(statusbar_container, NULL);
    //lv_img_set_src(img1, &battery_3);
}

void jolt_gui_create(lv_indev_t *kp_indev) {
    /* Set Jolt ssd1306 theme */
    lv_theme_t *th = lv_theme_jolt_init(100, NULL);
    lv_theme_set_current(th);  

    /* Create Groups for user input */
    g_main = lv_group_create();
    g_back = lv_group_create();
    lv_indev_set_group(kp_indev, g_main);
    // Don't need to set group since indev driver sends direct keypresses
    lv_obj_t * btn_back = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, back_release_action);
    lv_group_add_obj(g_back, btn_back);

    /* Create StatusBar */
    statusbar_create();

	/*Create the list*/
    main_menu_list = menu_create("Main", NULL, "Nano", jolt_gui_pin_create);
	lv_list_add(main_menu_list, NULL, "Bitcoin", list_release_action);
	lv_list_add(main_menu_list, NULL, "Ethereum", list_release_action);
	lv_list_add(main_menu_list, NULL, "Monero", list_release_action);
	lv_list_add(main_menu_list, NULL, "Console", list_release_action);
	lv_list_add(main_menu_list, NULL, "Settings", jolt_gui_settings_create);

    MSG("main_menu_list: %p\n", main_menu_list);
}
