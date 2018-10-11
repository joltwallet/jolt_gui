#include "jolt_gui.h"
#include "stdio.h"
#include "lv_theme_jolt.h"
#include "jolt_gui_entry.h"
#include "jolt_gui_symbols.h"

#if PC_SIMULATOR
    #include "test_stubs.h"
    #include "test_screens.h"
#elif ESP_PLATFORM
    //todo include required jolt libraries here
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_action_t back_release_action(lv_obj_t *btn);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *statusbar_container;
static lv_obj_t *statusbar_indicators;
static lv_obj_t *main_menu_list;

/**********************
 *      MACROS
 **********************/
#if PC_SIMULATOR
    #define MSG(...) printf(__VA_ARGS__)
#elif ESP_PLATFORM
    #include "esp_log.h"
    static const char TAG[] = "jolt_gui"
    #define MSG(...) ESP_LOGI(TAG, __VA_ARGS__)
#else
    #define MSG(...) printf(__VA_ARGS__)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
bool jolt_gui_delete_current_screen() {
    lv_obj_t *scrn = lv_group_get_focused(jolt_gui_store.group.main);
    if( NULL == scrn ) {
        MSG("Nothing in focus\n");
        return 0;
    }
    if( scrn == main_menu_list ) {
        MSG("Can't exit main menu\n");
        return 0;
    }

    // Disable any focus callback
    lv_group_set_focus_cb(jolt_gui_store.group.main, NULL);
    jolt_gui_store.digit.pos = -1; // just to be doubly sure
    lv_obj_t *parent = scrn;
    lv_obj_t *tmp = scrn;
    while( (tmp = lv_obj_get_parent(tmp)) ) {
        if( tmp != lv_scr_act() ) {
            parent = tmp;
        }
        if( tmp == main_menu_list ) {
            return false;
        }
    }
    MSG("deleting %p\n", parent);
    lv_obj_del(parent);
    return true;
}

static lv_action_t back_release_action(lv_obj_t *btn) {
    lv_obj_t *submenu = lv_group_get_focused(jolt_gui_store.group.main);

    if( jolt_gui_store.digit.pos <= 0 ) {
        /* Close the current container */
        jolt_gui_delete_current_screen();
    }
    else {
        /* Preserve the currently selected roller value */
        lv_roller_ext_t *ext = lv_obj_get_ext_attr(
                jolt_gui_store.digit.rollers[jolt_gui_store.digit.pos]);
        ext->ddlist.sel_opt_id_ori = ext->ddlist.sel_opt_id;

        /* Decrement position and refocus */
        jolt_gui_store.digit.pos--;
        MSG("Decrementing jolt_gui_store.digit.pos to %d\n",
                jolt_gui_store.digit.pos);
        lv_group_focus_obj(jolt_gui_store.digit.rollers[
                jolt_gui_store.digit.pos]);
    }
    return 0;
}

lv_obj_t *jolt_gui_parent_create() {
    lv_obj_t *parent = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(parent, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(parent,0,0);
    lv_obj_set_style(parent, &lv_style_transp);
    return parent;
}

lv_obj_t *jolt_gui_menu_create(const char *title, const void *img_src, 
        const char *txt, lv_action_t rel_action) {
    lv_obj_t *parent = jolt_gui_parent_create();

    /* Create List */
    lv_obj_t *menu = lv_list_create(parent, NULL);
    lv_obj_set_size(menu, LV_HOR_RES, LV_VER_RES - CONFIG_JOLT_GUI_STATUSBAR_H);
    lv_list_set_sb_mode(menu, LV_SB_MODE_AUTO);
    lv_obj_align(menu, NULL, 
            LV_ALIGN_IN_TOP_LEFT, 0, CONFIG_JOLT_GUI_STATUSBAR_H);
    lv_list_add(menu, img_src, txt, rel_action);
    lv_group_add_obj(jolt_gui_store.group.main, menu);
    lv_group_focus_obj(menu);

    /* Create and Stylize Statusbar Title */
    jolt_gui_title_create(parent, title);
    return menu;
}

lv_obj_t *jolt_gui_text_create(const char *title, const char *body) {
    /* Creates a page with scrollable text */
    lv_obj_t *parent = jolt_gui_parent_create();
    /* Create Statusbar Title */
    if( NULL != title ) {
        jolt_gui_title_create(parent, title);
    }

    /* Create Page */
    lv_obj_t *page = lv_page_create(parent, NULL);
    lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES - CONFIG_JOLT_GUI_STATUSBAR_H);
    lv_obj_align(page, NULL, LV_ALIGN_IN_TOP_LEFT,
            0, CONFIG_JOLT_GUI_STATUSBAR_H);
    lv_page_set_sb_mode(page, LV_SB_MODE_AUTO);
    lv_group_add_obj(jolt_gui_store.group.main, page);
    lv_page_set_arrow_scroll(page, true);

    /* Create Text Label on Page */
    lv_obj_t *label = lv_label_create(page, NULL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    {
        // Compute Width of Label
        lv_style_t *sb = lv_page_get_style(page, LV_PAGE_STYLE_SB);
        lv_obj_set_width(label, lv_page_get_scrl_width(page) - 
                sb->body.padding.inner - sb->body.padding.hor);  
    }
    lv_label_set_text(label, body);

    lv_group_focus_obj(page);

    return parent;
}

lv_obj_t *jolt_gui_title_create(lv_obj_t *parent, const char *title) {
    /* Creates the statusbar title label */
    if( NULL == parent ) {
        parent = lv_scr_act();
    }
    lv_obj_t *label = lv_label_create(parent, NULL);

    static lv_style_t label_style;
    lv_style_copy(&label_style, &lv_style_plain);
    label_style.body.main_color = LV_COLOR_WHITE;
    label_style.body.padding.ver = 0;
    label_style.body.padding.inner = 0;
    //label_style.text.font = &lv_font_dejavu_10;
    label_style.text.font = &lv_font_monospace_8;
    label_style.body.border.opa = LV_OPA_TRANSP;
    label_style.body.border.part = 0;

    lv_label_set_long_mode(label, LV_LABEL_LONG_ROLL);
    lv_label_set_body_draw(label, true); // draw background
    lv_label_set_style(label, &label_style);
    lv_obj_align(label, statusbar_container, LV_ALIGN_IN_LEFT_MID, 2, 0);
    lv_label_set_text(label, title);
    lv_obj_set_size(label, CONFIG_JOLT_GUI_TITLE_W, label_style.text.font->h_px);

    return label;
}

static void statusbar_update() {

    char statusbar_symbols[20] = { 0 };
    char *ptr = statusbar_symbols;

    uint8_t lock_status;
    lock_status = get_lock_status();
    if( lock_status == 0 ) {
    }
    else {
        strcpy(ptr, JOLT_GUI_SYMBOL_LOCK);
        ptr += 3;
    }

    uint8_t bluetooth_level;
    bluetooth_level = get_bluetooth_level();
    if( bluetooth_level == 0 ){
    }
    else {
        strcpy(ptr, JOLT_GUI_SYMBOL_BLUETOOTH);
        ptr += 3;
    }

    uint8_t wifi_level;
    wifi_level = get_wifi_level();
    if( wifi_level == 0 ) {
    }
    else if (wifi_level <= 55) {
        strcpy(ptr, JOLT_GUI_SYMBOL_WIFI_3);
        ptr += 3;
    }
    else if (wifi_level <= 75) {
        strcpy(ptr, JOLT_GUI_SYMBOL_WIFI_2);
        ptr += 3;
    }
    else {
        strcpy(ptr, JOLT_GUI_SYMBOL_WIFI_1);
        ptr += 3;
    }

    uint8_t battery_level = get_battery_level();
    MSG("Battery_level: %d\n", battery_level);
    if( battery_level > 100 ) {
        strcpy(ptr, JOLT_GUI_SYMBOL_BATTERY_CHARGING);
    }
    else if( battery_level > 70 ) {
        strcpy(ptr, JOLT_GUI_SYMBOL_BATTERY_3);
    }
    else if( battery_level > 45 ) {
        strcpy(ptr, JOLT_GUI_SYMBOL_BATTERY_2);
    }
    else if( battery_level > 15 ) {
        strcpy(ptr, JOLT_GUI_SYMBOL_BATTERY_1);
    }
    else {
        strcpy(ptr, JOLT_GUI_SYMBOL_BATTERY_EMPTY);
    }
    ptr += 3;

    lv_label_set_text(statusbar_indicators, statusbar_symbols);
    lv_obj_align(statusbar_indicators, statusbar_container,
            LV_ALIGN_IN_RIGHT_MID, -1, 0);
}

static void statusbar_create() {
    /* Create StatusBar Container */
    static lv_style_t header_style;
    statusbar_container = lv_cont_create(lv_scr_act(), NULL);
    lv_style_copy(&header_style, lv_cont_get_style(statusbar_container) );
    header_style.body.border.part = LV_BORDER_BOTTOM;
    header_style.body.border.color = LV_COLOR_BLACK;

    lv_cont_set_style(statusbar_container, &header_style);
    lv_obj_set_size(statusbar_container, LV_HOR_RES, CONFIG_JOLT_GUI_STATUSBAR_H);

    static lv_style_t status_style;
    lv_style_copy(&status_style, &header_style);
    status_style.text.font = &jolt_gui_symbols;
    status_style.body.padding.hor = 1;

    statusbar_indicators = lv_label_create(statusbar_container, NULL);
    lv_label_set_style(statusbar_indicators, &status_style);
    lv_obj_set_size(statusbar_indicators,
            LV_HOR_RES - CONFIG_JOLT_GUI_TITLE_W, 
            status_style.text.font->h_px);

    /* Periodically update the statusbar symbols */
    statusbar_update();
    lv_task_create(&statusbar_update, 2000, LV_TASK_PRIO_LOW, NULL);
}

static void group_mod_cb(lv_style_t *style) {
    style->body.border.part = 0;
}

void jolt_gui_create(lv_indev_t *kp_indev) {
    /* Set Jolt ssd1306 theme */
    //lv_theme_t *th = lv_theme_jolt_init(100, &orange_kid);
    //lv_theme_t *th = lv_theme_jolt_init(100, &lv_font_monospace_8);
    lv_theme_t *th = lv_theme_jolt_init(100, &f_6x10);
    //lv_theme_t *th = lv_theme_jolt_init(100, &synchronizer7);
    lv_theme_set_current(th);  

    /* Create Groups for user input */
    jolt_gui_store.group.main = lv_group_create();
    jolt_gui_store.group.back = lv_group_create();
    lv_indev_set_group(kp_indev, jolt_gui_store.group.main);
    lv_group_set_style_mod_cb(jolt_gui_store.group.main, group_mod_cb);

    // Don't need to set group since indev driver sends direct keypresses
    lv_obj_t *btn_back = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_action(btn_back, LV_BTN_ACTION_CLICK, &back_release_action);
    lv_group_add_obj(jolt_gui_store.group.back, btn_back);

    /* Create StatusBar */
    statusbar_create();

    /*Create the list*/
#if PC_SIMULATOR
    main_menu_list = jolt_gui_menu_create("Main", NULL, "PIN Entry",
            jolt_gui_pin_create);
    lv_list_add(main_menu_list, NULL, "Dummy", list_release_action);
    lv_list_add(main_menu_list, NULL, "Dummy", list_release_action);
    lv_list_add(main_menu_list, NULL, "Dummy", list_release_action);
    lv_list_add(main_menu_list, NULL, "Text Test", jolt_gui_test_text_create);
    lv_list_add(main_menu_list, NULL, "Submenu", jolt_gui_test_submenu_create);
#endif

    MSG("main_menu_list: %p\n", main_menu_list);
}
