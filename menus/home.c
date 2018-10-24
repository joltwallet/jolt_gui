#include "jolt_gui/jolt_gui.h"
#include "jolt_gui/lv_theme_jolt.h"
#include "settings.h"
#include "syscore/filesystem.h"
#include "syscore/launcher.h"
#include "jolt_helpers.h"

#include "esp_log.h"
#include "jolt_gui/jolt_gui_qr.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_action_t jolt_gui_test_qrcode_create(lv_obj_t *btn);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char TAG[] = "menu_home";


static lv_action_t woof(lv_obj_t btn) {
    printf("woof\n");
    return 0;
}

static lv_action_t back_release_action(lv_obj_t *btn) {
    jolt_gui_delete_current_screen();
    return 0;
}

static lv_action_t launch_file_proxy(lv_obj_t *btn) {
    char *fn = lv_list_get_btn_text( btn );
    ESP_LOGI(TAG, "Launching %s", fn);
    launch_file(fn, "app_main", 0, NULL);
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
        // Find and Register all user apps
        char **fns = NULL;
        uint16_t n_fns = jolt_fs_get_all_elf_fns( &fns );

        const char TITLE[] = "Main";

        if( n_fns > 0 ) {
            ESP_LOGI(TAG, "Registering App \"%s\" into the GUI", fns[0]);
            jolt_gui_store.main_menu_list = jolt_gui_menu_create(TITLE, NULL, fns[0], launch_file_proxy);
            for(uint16_t i=1; i<n_fns; i++) {
                ESP_LOGD(TAG, "Registering App \"%s\" into the GUI", fns[i]);
                lv_list_add(jolt_gui_store.main_menu_list, NULL, fns[i], launch_file_proxy);
            }
            lv_list_add(jolt_gui_store.main_menu_list, NULL, "Settings", menu_settings_create);
        }
        else {
            jolt_gui_store.main_menu_list = jolt_gui_menu_create(TITLE, NULL, "Settings", menu_settings_create);
        }

        lv_list_add(jolt_gui_store.main_menu_list, NULL, "QR", jolt_gui_test_qrcode_create);
        lv_list_add(jolt_gui_store.main_menu_list, NULL, "Dummy 1", NULL);
        lv_list_add(jolt_gui_store.main_menu_list, NULL, "Dummy 2", NULL);
        lv_list_add(jolt_gui_store.main_menu_list, NULL, "Dummy 3", NULL);
        lv_list_add(jolt_gui_store.main_menu_list, NULL, "Dummy 4", NULL);
        lv_list_add(jolt_gui_store.main_menu_list, NULL, "Dummy 5", NULL);
        lv_list_add(jolt_gui_store.main_menu_list, NULL, "Dummy 6", NULL);

        jolt_h_free_char_array(fns, n_fns);
    }
#endif
}

static lv_action_t jolt_gui_test_qrcode_create(lv_obj_t *btn) {
    const char data[] = "Meow";
    jolt_gui_scr_qr_create("QR Test", "Meow", sizeof(data));

#if 0
    QRCode qrcode;
    // qrcode_bytes is referenced in QRCode
    uint8_t qrcode_bytes[qrcode_getBufferSize(CONFIG_JOLT_QR_VERSION)];
    qrcode_initText(&qrcode, qrcode_bytes, CONFIG_JOLT_QR_VERSION,
                        ECC_LOW, buf);

	lv_img_dsc_t *img = jolt_gui_qr_to_img_dsc(&qrcode);
	lv_obj_t *scr = jolt_gui_qr_fullscreen_create(img, 1, "QR Test");

    // todo: free the img buffer
    //
#endif
	return 0;
}
