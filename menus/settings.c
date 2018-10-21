/* Jolt Wallet - Open Source Cryptocurrency Hardware Wallet
 Copyright (C) 2018  Brian Pugh, James Coxon, Michael Smaili
 https://www.joltwallet.com/
 */

#include "esp_log.h"

#if 0
#include "../loading.h"
#include "../entry.h"
#include "../../globals.h"
#include "../../vault.h"
#include "../../gui/gui.h"
#endif
//#include "../../helpers.h"
#include "../../radio/wifi.h"
#include "hal/storage/storage.h"
#include "jolt_gui/jolt_gui.h"

static const char TAG[] = "menu_settings";


static lv_action_t menu_wifi_details_create(lv_action_t *btn) {
    char new_ap_info[45];
    get_ap_info(new_ap_info, sizeof(new_ap_info));
    lv_obj_t *t = jolt_gui_text_create("WiFi Details", new_ap_info);
    return 0;
}

static lv_action_t menu_factory_reset_create(lv_obj_t *btn) {
    lv_obj_t *l = jolt_gui_menu_create("Factory Reset?",  NULL, "No", NULL);
    lv_list_add(l, NULL, "Yes", NULL);
    //storage_factory_reset();
    return 0;
}

#define SCREEN_BRIGHTNESS_DELTA 25

#if 0
static void screen_brightness_callback(uint8_t brightness){
    SCREEN_MUTEX_TAKE;
    u8g2_SetContrast(u8g2, brightness);
    SCREEN_MUTEX_GIVE;
}
#endif

static lv_action_t menu_screen_brightness_create() {
    const char title[] = "Brightness";
    //uint8_t brightness = get_display_brightness();

    //entry_slider_callback(menu, &brightness, SCREEN_BRIGHTNESS_DELTA, title,
    //            &screen_brightness_callback);
    // save_display_brightness(brightness);
    return 0;
}

lv_action_t menu_settings_create(lv_obj_t *btn) {
    //ESP_LOGI(TAG, "meow");
    printf("menu settings\n");
    lv_obj_t *l = jolt_gui_menu_create("Settings",  NULL, 
            "WiFi", menu_wifi_details_create);
    lv_list_add(l, NULL, "Screen Brightness", menu_screen_brightness_create);
    lv_list_add(l, NULL, "Bluetooth", NULL);
    lv_list_add(l, NULL, "Long Option Name Scrolls", NULL);
    lv_list_add(l, NULL, "Factory Reset", menu_factory_reset_create);
    return 0;
}

#if 0
void menu_settings(menu8g2_t *prev){
    menu8g2_t menu;
    menu8g2_copy(&menu, prev);
    const char title[] = "Settings";

    menu8g2_elements_t elements;
    menu8g2_elements_init(&elements, 4);
    menu8g2_set_element(&elements, "Screen Brightness", &screen_brightness);
    menu8g2_set_element(&elements, "WiFi Details", &wifi_details);
    menu8g2_set_element(&elements, "Bluetooth", NULL);
    menu8g2_set_element(&elements, "Factory Reset", &menu_factory_reset);
    menu8g2_create_vertical_element_menu(&menu, title, &elements);
    menu8g2_elements_free(&elements);
}
#endif
