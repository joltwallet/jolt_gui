#include "jolt_gui.h"
#include "jolt_gui_first_boot.h"

#include "bipmnemonic.h"
#include "esp_log.h"
#include "jolttypes.h"

#include "sodium.h"
#include "hal/storage/storage.h"

#if CONFIG_JOLT_STORE_ATAES132A
#include "aes132_cmd.h"
#endif

#define MNEMONIC_STRENGTH 256

static const char TAG[] = "first_boot";
static CONFIDENTIAL uint256_t mnemonic_bin;
static CONFIDENTIAL char mnemonic[BM_MNEMONIC_BUF_LEN];

static void generate_mnemonic();
static lv_action_t stretch_cb(lv_obj_t *dummy);
static lv_action_t screen_pin_verify_create(lv_obj_t *num);
static lv_action_t screen_mnemonic_create(lv_obj_t *btn);

static lv_action_t screen_pin_entry_create(lv_obj_t *btn);
static jolt_err_t get_nth_word(char buf[], size_t buf_len,
        const char *str, const uint32_t n);

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

static void generate_mnemonic() {
    bm_entropy256(mnemonic_bin);
#if CONFIG_JOLT_STORE_ATAES132A
    {
        /* Mix in entropy from ataes132a */
        CONFIDENTIAL uint256_t aes132_entropy;
        uint8_t res = aes132_rand(aes132_entropy, sizeof(aes132_entropy));
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
}

static lv_action_t stretch_cb(lv_obj_t *dummy) {
    storage_set_mnemonic(mnemonic_bin, jolt_gui_store.derivation.pin);
    storage_set_pin_count(0); // Only does something if pin_count is setable
    uint32_t pin_count = storage_get_pin_count();
    storage_set_pin_last(pin_count);

    sodium_memzero(jolt_gui_store.derivation.pin, sizeof(jolt_gui_store.derivation.pin));
    sodium_memzero(mnemonic_bin, sizeof(mnemonic_bin));
    sodium_memzero(mnemonic, sizeof(mnemonic));

    esp_restart();

    return 0; // Should never reach here
}

static lv_action_t mismatch_cb(lv_obj_t *btn) {
    // Delete text screen
    lv_obj_del(lv_obj_get_parent(btn));
    // Create pin entry screen
    screen_pin_entry_create(NULL);
    return 0;
}

static lv_action_t screen_finish_create(lv_obj_t *num) {
    CONFIDENTIAL static uint256_t pin_hash_verify;
    jolt_gui_num_get_hash(num, pin_hash_verify);

    // Delete verify pin entry screen
    lv_obj_del(lv_obj_get_parent(num));

    int res = memcmp(jolt_gui_store.derivation.pin, pin_hash_verify, sizeof(pin_hash_verify));
    sodium_memzero(pin_hash_verify, sizeof(pin_hash_verify));

    // Verify the pins match
    if( 0 ==  res ){
        jolt_gui_stretch("Setup", "Saving PIN", jolt_gui_store.derivation.pin, stretch_cb);
    }
    else{
        sodium_memzero(jolt_gui_store.derivation.pin, sizeof(jolt_gui_store.derivation.pin));
        lv_obj_t *scr = jolt_gui_scr_text_create("Pin Setup", "Pin Mismatch! Please try again.");
        jolt_gui_scr_set_back_action(scr, mismatch_cb);
    }

    return 0;
}

static lv_action_t screen_pin_verify_create(lv_obj_t *num) {
    jolt_gui_num_get_hash(num, jolt_gui_store.derivation.pin); // compute hash for first pin entry screen
    // Delete original pin entry screen
    lv_obj_del(lv_obj_get_parent(num));
    jolt_gui_scr_num_create( "PIN Verify", CONFIG_JOLT_GUI_PIN_LEN,
            JOLT_GUI_NO_DECIMAL, &screen_finish_create); 
    return 0;
}

static lv_action_t screen_pin_entry_create(lv_obj_t *btn) {
    lv_obj_t *screen = jolt_gui_scr_num_create( "PIN",
            CONFIG_JOLT_GUI_PIN_LEN, JOLT_GUI_NO_DECIMAL, screen_pin_verify_create);
    return 0;
}

static lv_action_t screen_mnemonic_create(lv_obj_t *btn) {
    const char title[] = "Write Down Mnemonic!";
    lv_obj_t *scr = jolt_gui_scr_menu_create(title);
    for(uint8_t i=0; i < 24; i++) {
        char buf[15] = { 0 }; // todo define
        get_nth_word(buf, sizeof(buf), mnemonic, i);
        if( i == 0 ) {
            jolt_gui_scr_menu_add(scr, NULL, buf, NULL);
        }
        else {
            jolt_gui_scr_menu_add(scr, NULL, buf, NULL);
        }
    }
    jolt_gui_scr_menu_add(scr, NULL, "continue",
            screen_pin_entry_create);
    return 0;
}

/* Called externally to begin the first-boot GUI */
void jolt_gui_first_boot_create() {
    generate_mnemonic();

    lv_obj_t *scr = jolt_gui_scr_text_create( "First Startup",
            "Welcome to Jolt, "
            "please backup the following secret mnemonic.");
    jolt_gui_scr_set_enter_action(scr, &screen_mnemonic_create);
    jolt_gui_scr_set_back_action(scr, NULL);
}

