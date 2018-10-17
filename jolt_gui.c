#include "jolt_gui.h"
#include "stdio.h"
#include "lv_theme_jolt.h"
#include "jolt_gui_entry.h"
#include "jolt_gui_symbols.h"
#include "jolt_gui_statusbar.h"

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
static lv_obj_t *main_menu_list;

/**********************
 *      MACROS
 **********************/
#if PC_SIMULATOR
    #define MSG(...) printf(__VA_ARGS__)
#elif ESP_PLATFORM
    #include "esp_log.h"
    static const char TAG[] = "jolt_gui";
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
    jolt_gui_delete_current_screen();
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

    jolt_gui_set_enter_action(parent, jolt_gui_fwd_main);
    jolt_gui_set_back_action(parent, jolt_gui_delete_current_screen);
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
    label_style.text.font = &lv_font_monospace_8;
    label_style.body.border.opa = LV_OPA_TRANSP;
    label_style.body.border.part = 0;

    lv_label_set_long_mode(label, LV_LABEL_LONG_ROLL);
    lv_label_set_body_draw(label, true); // draw background
    lv_label_set_style(label, &label_style);
    lv_obj_align(label, jolt_gui_store.statusbar.container,
            LV_ALIGN_IN_LEFT_MID, 2, 0);
    lv_label_set_text(label, title);
    lv_obj_set_size(label, CONFIG_JOLT_GUI_TITLE_W, label_style.text.font->h_px);

    return label;
}

static void group_mod_cb(lv_style_t *style) {
    style->body.border.part = 0;
}

void jolt_gui_group_create() {
    /* Create Groups for user input */
    jolt_gui_store.group.main = lv_group_create();
    jolt_gui_store.group.back = lv_group_create();
    jolt_gui_store.group.enter = lv_group_create();
    lv_group_set_style_mod_cb(jolt_gui_store.group.main, group_mod_cb);
}
static lv_action_t woof(lv_obj_t btn) {
    printf("woof\n");
    return 0;
}

// todo delete this
static lv_res_t list_release_action(lv_obj_t * list_btn) {
    /* PLACEHOLDER STUB*/
    printf("List element click:%s\n", lv_list_get_btn_text(list_btn));
    return LV_RES_OK; /*Return OK because the list is not deleted*/
}

// todo delete this
static lv_res_t jolt_gui_test_submenu_create(lv_obj_t * list_btn) {
    /*Create the list*/
    lv_obj_t *settings_list = jolt_gui_menu_create("Settings",
            NULL, "WiFi", list_release_action);
    lv_list_add(settings_list, NULL, "Bluetooth", list_release_action);
    lv_list_add(settings_list, NULL, "Long Option Name Scrolls",
            list_release_action);
    lv_list_add(settings_list, NULL, "Factory Reset", list_release_action);
    return LV_RES_OK;
}

void jolt_gui_create() {
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
    main_menu_list = jolt_gui_menu_create("Main", NULL, "PIN Entry",
            jolt_gui_test_pin_create);
    lv_list_add(main_menu_list, NULL, "Loading Test", jolt_gui_test_loading_create);
    lv_list_add(main_menu_list, NULL, "Alphabet", jolt_gui_test_alphabet_create);
    lv_list_add(main_menu_list, NULL, "Numeric Begin", jolt_gui_test_numeric_begin_dp_create);
    lv_list_add(main_menu_list, NULL, "Numeric End", jolt_gui_test_numeric_end_dp_create);
    lv_list_add(main_menu_list, NULL, "Numeric Mid", jolt_gui_test_numeric_mid_dp_create);
    lv_list_add(main_menu_list, NULL, "Text Test", jolt_gui_test_text_create);
    lv_list_add(main_menu_list, NULL, "Submenu", jolt_gui_test_submenu_create);
#elif ESP_PLATFORM
    if( true && jolt_gui_store.first_boot ) {
        jolt_gui_first_boot_create();
    }
    else {
        main_menu_list = jolt_gui_menu_create("Main", NULL, "PIN Entry", woof);
        lv_list_add(main_menu_list, NULL, "submenu", jolt_gui_test_submenu_create);
        lv_list_add(main_menu_list, NULL, "Dummy 1", NULL);
        lv_list_add(main_menu_list, NULL, "Dummy 2", NULL);
        lv_list_add(main_menu_list, NULL, "Dummy 3", NULL);
        lv_list_add(main_menu_list, NULL, "Dummy 4", NULL);
        lv_list_add(main_menu_list, NULL, "Dummy 5", NULL);
        lv_list_add(main_menu_list, NULL, "Dummy 6", NULL);
        lv_list_add(main_menu_list, NULL, "Dummy 7", NULL);
    }
#endif
}

lv_obj_t *jolt_gui_set_back_action(lv_obj_t *parent, lv_action_t cb) {
    if( NULL == parent ) {
        parent = lv_scr_act();
    }
    lv_obj_t *btn = lv_btn_create(parent, NULL);
    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, cb);
    lv_obj_set_size(btn, 0, 0);
    lv_group_remove_obj(btn);
    lv_group_add_obj(jolt_gui_store.group.back, btn);
    lv_group_focus_obj(btn);
    return btn;
}

lv_obj_t *jolt_gui_set_enter_action(lv_obj_t *parent, lv_action_t cb) {
    if( NULL == parent ) {
        parent = lv_scr_act();
    }
    lv_obj_t *btn = lv_btn_create(parent, NULL);
    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, cb);
    lv_obj_set_size(btn, 0, 0);
    lv_group_remove_obj(btn);
    lv_group_add_obj(jolt_gui_store.group.enter, btn);
    lv_group_focus_obj(btn);
    return btn;
}

lv_action_t jolt_gui_fwd_main(lv_obj_t *btn) {
    printf("boop\n");
    lv_group_send_data(jolt_gui_store.group.main, LV_GROUP_KEY_ENTER);
    return 0;
}
