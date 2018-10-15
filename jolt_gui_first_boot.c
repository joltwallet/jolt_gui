#include "jolt_gui.h"
#include "jolt_gui_first_boot.h"

#include "bipmnemonic.h"
#include "esp_log.h"
#include "jolttypes.h"

#if CONFIG_JOLT_STORE_ATAES132A
#include "aes132_cmd.h"
#endif

#define MNEMONIC_STRENGTH 256

static uint8_t scr_idx = 0;
static lv_action_t next_screen(lv_obj_t *btn);
static lv_action_t prev_screen(lv_obj_t *btn);

static const char TAG[] = "first_boot";
static CONFIDENTIAL uint256_t mnemonic_bin;
static CONFIDENTIAL char mnemonic[BM_MNEMONIC_BUF_LEN];

static jolt_err_t get_nth_word(char buf[], size_t buf_len,
        const char *str, const uint32_t n){
    // Assumes a single space between words; no leading/trailing spaces
    // Copies the nth word of null-terminated str into buf.
    if ( (n+1)==25 ){
        strlcpy(buf, "Continue", buf_len);
        return E_SUCCESS;
    }

    // Copy over number prefix
    sprintf(buf, "%d. ", n+1); // 1-indexing
    buf_len -= 3;
    buf += 3;
    if( (n+1)>9 ){
        buf_len--;
        buf++;
    }

    // Copy over the nth word
    for(uint8_t i = 0; str!=0; str++){
        if( i == n ){
            if(*str == ' ' || *str == '\0' || buf_len <= 1){
                *buf = '\0';
                return E_SUCCESS;
            }
            else{
                *buf = *str;
                buf++;
                buf_len--;
            }
        }
        else if (*str == ' '){
            i++;
        }
    }
    return E_FAILURE;
}

static void screen_create() {
    static lv_obj_t *current_screen = NULL;
    if( NULL != current_screen ) {
        // todo: securely delete the mnemonic list lv_obj_t
        lv_obj_del(current_screen);
        current_screen = NULL;
    }

    switch( scr_idx ) {
        case(0):{
            current_screen = jolt_gui_text_create( "First Startup",
                    "Welcome to Jolt, "
                    "please backup the following secret mnemonic.");
            jolt_gui_set_back_action(current_screen, NULL);
            jolt_gui_set_enter_action(current_screen, &next_screen);
            break;
        }
        case(1):{
            const char title[] = "Write Down Mnemonic!";
            for(uint8_t i=0; i < 24; i++) {
                char buf[15] = { 0 }; // todo define
                get_nth_word(buf, sizeof(buf), mnemonic, i);
                if( i == 0 ) {
                    current_screen = jolt_gui_menu_create(title, NULL, 
                            buf, NULL);
                }
                else {
                    lv_list_add(current_screen, NULL, buf, NULL);
                }
            }
            lv_list_add(current_screen, NULL, "continue", NULL);
            jolt_gui_set_back_action(current_screen, &prev_screen);
            break;
        }
    }
}

static lv_action_t next_screen(lv_obj_t *btn) {
    scr_idx++;
    screen_create();
    return 0;
}

static lv_action_t prev_screen(lv_obj_t *btn) {
    if( scr_idx > 0 ) {
        scr_idx--;
        screen_create();
    }
    return 0;
}

/* Called externally to begin the first-boot GUI */
void jolt_gui_first_boot_create() {

    bm_entropy256(mnemonic_bin);
#if CONFIG_JOLT_STORE_ATAES132A
    {
        /* Mix in entropy from ataes132a */
        CONFIDENTIAL uint256_t aes132_entropy;
        uint8_t res;
        res = aes132_rand(aes132_entropy, sizeof(aes132_entropy));
        if( res ) {
            esp_restart();
        }
        for(uint8_t i=0; i<sizeof(aes132_entropy); i++) {
            mnemonic_bin[i] ^= aes132_entropy[i];
        }
        sodium_memzero(aes132_entropy, sizeof(aes132_entropy));
    }
#endif
    bm_bin_to_mnemonic(mnemonic, sizeof(mnemonic), mnemonic_bin,
            MNEMONIC_STRENGTH);
    ESP_LOGI(TAG, "mnemonic %s", mnemonic);

    screen_create();
}

