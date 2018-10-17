#include "jolt_gui.h"
#include "jolt_gui_first_boot.h"

#include "bipmnemonic.h"
#include "esp_log.h"
#include "jolttypes.h"

#include "sodium.h"

#if CONFIG_JOLT_STORE_ATAES132A
#include "aes132_cmd.h"
#endif

#define MNEMONIC_STRENGTH 256

static const char TAG[] = "first_boot";
static CONFIDENTIAL uint256_t mnemonic_bin;
static CONFIDENTIAL char mnemonic[BM_MNEMONIC_BUF_LEN];
static CONFIDENTIAL uint256_t pin_hash;

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

/* Collects the information on the rollers, computes the hash */
static void compute_hash(uint256_t hash) {
    uint8_t pin_array[CONFIG_JOLT_GUI_PIN_LEN] = { 0 };
    // Parse and zero out the pin array
    printf("Entered PIN: ");
    for(uint8_t i=0; i < sizeof(pin_array); i++) {
        pin_array[i] = 9 - (lv_roller_get_selected(jolt_gui_store.digit.rollers[i]) % 10);
        //todo: securely zero out the rollers
        //pin_array[i] = lv_roller_get_selected(jolt_gui_store.digit.rollers[i]);
        printf("%d ", pin_array[i]);
    }
    printf("\n");

    /* Convert pin into a 256-bit key */
    crypto_generichash_blake2b_state hs;
    crypto_generichash_init(&hs, NULL, 32, 32);
    crypto_generichash_update(&hs, (unsigned char *) pin_array,
            CONFIG_JOLT_GUI_PIN_LEN);
    crypto_generichash_final(&hs, hash, 32);
}

static void generate_mnemonic() {
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
}

static lv_action_t screen_finish_create(lv_obj_t *btn) {
    CONFIDENTIAL static uint256_t pin_hash_verify;
    compute_hash(pin_hash_verify);
    // Verify the pins match
    if( 0 == memcmp(pin_hash, pin_hash_verify, sizeof(pin_hash_verify)) ){
        sodium_memzero(pin_hash_verify, sizeof(pin_hash_verify));
        // todo: store and reboot
    }
    else{
        sodium_memzero(pin_hash_verify, sizeof(pin_hash_verify));
        jolt_gui_text_create("Pin Setup", 
                "Pin Mismatch! Please try again.");
    }

    return 0;
}

static lv_action_t screen_pin_verify_create(lv_obj_t *btn) {
    compute_hash(pin_hash); // compute hash for first pin entry screen

    jolt_gui_numeric_create( CONFIG_JOLT_GUI_PIN_LEN,
            JOLT_GUI_NO_DECIMAL, "PIN Verify", &screen_finish_create); 
    return 0;
}

static lv_action_t screen_pin_entry_create(lv_obj_t *btn) {
    jolt_gui_numeric_create( CONFIG_JOLT_GUI_PIN_LEN,
            JOLT_GUI_NO_DECIMAL, "PIN Setup", &screen_pin_verify_create); 
    return 0;
}

static lv_action_t screen_mnemonic_create(lv_obj_t *btn) {
    const char title[] = "Write Down Mnemonic!";
    lv_obj_t *scr = NULL;
    for(uint8_t i=0; i < 24; i++) {
        char buf[15] = { 0 }; // todo define
        get_nth_word(buf, sizeof(buf), mnemonic, i);
        if( i == 0 ) {
            scr = jolt_gui_menu_create(title, NULL,
                    buf, NULL);
            // debug: move this to else statement
            lv_list_add(scr, NULL, "continue",
                    &screen_pin_entry_create);
        }
        else {
            lv_list_add(scr, NULL, buf, NULL);
        }
    }
    return 0;
}

/* Called externally to begin the first-boot GUI */
void jolt_gui_first_boot_create() {
    generate_mnemonic();

    lv_obj_t *scr = jolt_gui_text_create( "First Startup",
            "Welcome to Jolt, "
            "please backup the following secret mnemonic.");
    jolt_gui_set_enter_action(scr, &screen_mnemonic_create);
}

