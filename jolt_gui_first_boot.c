#include "jolt_gui.h"
#include "jolt_gui_first_boot.h"

#if CONFIG_JOLT_STORE_ATAES132A
#include "aes132_cmd.h"
#endif

static uint8_t scr_idx = 0;
static lv_action_t enter_action(lv_obj_t *btn);
static lv_action_t back_action(lv_obj_t *btn);

static void screen_create() {
    static lv_obj_t *current_screen = NULL;
    if( NULL != current_screen ) {
        lv_obj_del(current_screen);
        current_screen = NULL;
    }

    switch( scr_idx ) {
        case(0):{
            current_screen = jolt_gui_text_create( "First Startup",
                    "Welcome to Jolt, "
                    "please backup the following secret mnemonic.");
            break;
        }
        case(1):{
            const char title[] = "Write Down Mnemonic!";
            current_screen = jolt_gui_text_create( "First Startup",
                    "hello" );
#if 0
            if( menu8g2_create_vertical_menu(menu, title, mnemonic,
                    (void *)&get_nth_word, 25) ){
                if(menu->index == 24){
                    current_screen++;
                }
            }
            else{
                current_screen--;
            }
#endif
            break;
        }
    }
    jolt_gui_set_back_action(current_screen, back_action);
    jolt_gui_set_enter_action(current_screen, enter_action);
}

static lv_action_t enter_action(lv_obj_t *btn) {
    scr_idx++;
    screen_create();
    return 0;
}

static lv_action_t back_action(lv_obj_t *btn) {
    if( scr_idx > 0 ) {
        scr_idx--;
        screen_create();
    }
    return 0;
}

/* Called externally to begin the first-boot GUI */
void jolt_gui_first_boot_create() {
    screen_create();
}

