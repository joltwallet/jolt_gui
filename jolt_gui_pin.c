#include "jolt_gui_pin.h"
#include "jolt_gui.h"
#include "hal/storage/storage.h"
#include "sodium.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static const char TAG[] = "JoltGuiPIN";
static lv_action_t back_cb;
static lv_action_t enter_cb;

/* Gets triggered when user presses back on the "incorrect pin" screen */
static lv_action_t pin_fail_cb( lv_obj_t *scr ) {
    /* Recreate the pin entry screen */
    jolt_gui_scr_pin_create(NULL, NULL);
    /* Delete the "incorrect pin" screen */
    lv_obj_del(scr);
    return 0;
}

/* gets triggered when the user presses enter on the last pin roller on a pin
 * screen */
static lv_action_t pin_enter(lv_obj_t *num) {
    // todo: actually make this a loading screen
    /* Get the pin_hash from the pin screen */
    CONFIDENTIAL uint256_t pin_hash;
    jolt_gui_num_get_hash(num, pin_hash);

    /* Delete the Pin Entry Screen */
    lv_obj_del(lv_obj_get_parent(num));

    // todo: spawn a task to do the stretching
    // todo: storage_get_mnemonic should take the stretched key
    
    /* storage_get_mnemonic inherently increments and stores the 
     * attempt counter*/
    bool unlock_res = storage_get_mnemonic(
            jolt_gui_store.tmp.mnemonic_bin, pin_hash);
    sodium_memzero(pin_hash, sizeof(pin_hash));
    if( unlock_res ) {
        /* Correct PIN */
        ESP_LOGI(TAG, "Correct PIN");
        /* Record the current pin attempt as the last successful attempt */
        uint32_t pin_count = storage_get_pin_count();
        storage_set_pin_last(pin_count);
        /* Execute the user callback */
        if( NULL != enter_cb ) {
            enter_cb( NULL );
        }
        else {
            ESP_LOGE(TAG, "No Enter Callback");
        }
    }
    else{
        /* Wrong PIN */
        ESP_LOGE(TAG, "Incorrect PIN");

        /* Create a screen telling the user */
        jolt_gui_scr_text_create("PIN", "Wrong PIN");

        /* Delete the PIN Screen */
        lv_obj_del(lv_obj_get_parent(num));
    }
    return 0;
}

static lv_action_t pin_back_cb( lv_obj_t *num ) {
    if( NULL != back_cb ) {
        back_cb( NULL );
        back_cb = NULL;
        enter_cb = NULL;
    }
    else {
        ESP_LOGE(TAG, "No Back Callback");
    }
    lv_obj_del(lv_obj_get_parent(num));
    return 0;
}

lv_obj_t *jolt_gui_scr_pin_create(lv_action_t failure_cb, lv_action_t success_cb) {
    /* Primary Job: Prompt user for a pin, and on success to put the 
     * 256-bit mnemonic into jolt_gui_store.tmp.mnemonic_bin.
     *
     * It is host function's responsibility to ensure cleanup of mnemonic.
     *
     * Executes failure_cb if the user backs out of the pin entry screen.
     * Executes the success_cb if pin was correctly entered */
    char title[20];
    uint32_t pin_attempts = storage_get_pin_count() - storage_get_pin_last();
    if( pin_attempts >= CONFIG_JOLT_DEFAULT_MAX_ATTEMPT ) {
        storage_factory_reset();
    }
    /* Store the callbacks */
    if( NULL != failure_cb ) {
        back_cb = failure_cb;
    }
    if( NULL != success_cb ){
        enter_cb = success_cb;
    }

    sprintf(title, "Enter Pin (%d/%d)", pin_attempts+1,
            CONFIG_JOLT_DEFAULT_MAX_ATTEMPT);

    lv_obj_t *parent = jolt_gui_scr_num_create( title,
            CONFIG_JOLT_GUI_PIN_LEN, JOLT_GUI_NO_DECIMAL, pin_enter);

    lv_obj_t *numeric = jolt_gui_scr_num_get(parent);
    jolt_gui_num_set_back_action(numeric, pin_back_cb);

    return parent;
}

# if 0
bool storage_get_mnemonic(char *buf, const uint16_t buf_len) {
    /* prompt user for pin, returns 24-word mnemonic mnemonic from storage 
     * Returns true if mnemonic is returned; false if use binary mnemonic */

    uint32_t pin_last = storage_get_pin_last();
    uint32_t pin_count = storage_get_pin_count();
    

    for(;;) { // Loop will exit upon successful PIN or cancellation
        uint32_t pin_attempts = pin_count - pin_last;
        if( pin_attempts >= CONFIG_JOLT_DEFAULT_MAX_ATTEMPT ) {
            storage_factory_reset();
        }
        char title[20];
        sprintf(title, "Enter Pin (%d/%d)", pin_attempts+1,
                CONFIG_JOLT_DEFAULT_MAX_ATTEMPT);
        CONFIDENTIAL uint256_t pin_hash = { 0 };

        // 1) Create semaphore to signal when user is done
        // 2) If the pin_hash is still 0, exit
        // 3) If its not continue
        // actually, it really needs to trigger a callback upon completion;
        // no semaphore

        // Create pin screen

        // Wait until the user is done with the PIN screen
#if 0
        if( !entry_pin(menu, pin_hash, title) ) {
            // User cancelled vault operation by pressing back
            res = false;
            goto exit;
        }
#endif
        pin_count++;
        storage_set_pin_count(pin_count); // if this fails, it should reset device

        //loading_enable();
        //loading_text_title("Unlocking", TITLE);
        bool unlock_success;
        unlock_success = storage_get_mnemonic(bin, pin_hash);

        sodium_memzero(pin_hash, sizeof(pin_hash));
        //loading_disable();

        if( unlock_success ){ // Success
            storage_set_pin_last(pin_count);
            if( E_SUCCESS != bm_bin_to_mnemonic(buf, buf_len, bin, 256) ) {
                esp_restart();
            }
            sodium_memzero(bin, sizeof(bin));
            ESP_LOGI(TAG, "Correct PIN.");
            res = true;
            goto exit;
        }
        else{
            jolt_gui_scr_text_create(TITLE, "Wrong PIN");
        }
    }
exit:
    return res;
}
#endif

#if 0
/* BLOCKING FUNCTION
 * blocks and creates a screen 
 * Unlike other screen functions, its blocking and doesn't return an object*/
void jolt_gui_scr_pin(char *title, uint8_t *hash){
    lv_obj_t *scr = jolt_gui_scr_num_create( title, CONFIG_JOLT_GUI_PIN_LEN,
                JOLT_GUI_NO_DECIMAL, &screen_finish_create); 

    /* Iterate through children to get the numeric */
    lv_obj_t *child = NULL;
    lv_obj_t *numeric;
    lv_obj_type_t obj_type;
    while( NULL != (child = lv_obj_get_child(scr, child)) ) {
        lv_obj_get_type(child, &obj_type);
        if(strcmp("jolt_num", obj_type.type[0])) {
            numeric = child;
            break;
        }
    }

    SemaphoreHandle_t sig = xSemaphoreCreateBinary();
    if( NULL == sig ) {
        return false;
    }

    xSemaphoreTake(sig, portMAX_DELAY);
    vSemaphoreDelete( sig );

    jolt_gui_num_set_enter_action(numeric, lv_action_t cb);
    jolt_gui_num_set_back_action(numeric, lv_action_t cb);

}

#endif
