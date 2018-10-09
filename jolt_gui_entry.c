#include "jolt_gui.h"
#include "jolt_gui_entry.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define MSG(...) printf(__VA_ARGS__)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
extern int8_t numerical_entry_loc = -1;
//extern lv_obj_t **rollers;
extern uint8_t pin_spacing = 0;


static lv_group_focus_cb_t update_selected_roller_style(lv_obj_t *roller) {
    /* Only the focused roller has 3 visible selections;
     * all unselected rollers only have 1 */
    MSG("roller style callback!\n");
    if(numerical_entry_loc != 0 ) {
	    lv_obj_align(rollers[0], NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
    }
    for(uint8_t i=0; i<CONFIG_JOLT_GUI_PIN_LEN; i++){
        uint8_t n_visible = 1;
        if( i == numerical_entry_loc ) {
            n_visible = 3;
        }
        lv_roller_set_visible_row_count(rollers[i], n_visible);
        if( i > 0 ) {
	        lv_obj_align(rollers[i], rollers[i-1], LV_ALIGN_OUT_RIGHT_MID, 
                    pin_spacing, 0);
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


static lv_obj_t *digit_create(lv_obj_t *parent) {
    lv_obj_t *roller;
    roller = lv_roller_create(parent, NULL);
	lv_roller_set_visible_row_count(roller, 3);
    lv_roller_set_anim_time(roller, CONFIG_JOLT_GUI_ANIM_DIGIT_MS);
	lv_roller_set_options(roller, 
            "0\n1\n2\n3\n4\n5\n6\n7\n8\n9"
            "\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9"
            "\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9"
            );
    lv_roller_set_selected(roller, 10, false); // Set it to the middle 0 entry
    lv_roller_set_action(roller, digit_entry_cb);
    lv_group_add_obj(g_main, roller);
    return roller;
}


lv_res_t jolt_gui_pin_create( void ) { 
    lv_obj_t *parent = jolt_gui_parent_create();

    lv_obj_t *pin_container = lv_cont_create(parent, NULL);
    lv_obj_set_size(pin_container, LV_HOR_RES, 
            LV_VER_RES - CONFIG_JOLT_GUI_STATUSBAR_H);
	lv_obj_align(pin_container, NULL, 
            LV_ALIGN_IN_TOP_LEFT, 0, CONFIG_JOLT_GUI_STATUSBAR_H);

    rollers[0] = digit_create(pin_container);

    /* Compute Roller Spacing */
    if( 0 == pin_spacing ) {
        uint8_t roller_width = lv_obj_get_width(rollers[0]);
        pin_spacing = 
                ( lv_obj_get_width(pin_container) - 
                  CONFIG_JOLT_GUI_PIN_LEN*roller_width ) / 
                ( CONFIG_JOLT_GUI_PIN_LEN + 2 );
    }
	lv_obj_align(rollers[0], NULL, LV_ALIGN_IN_LEFT_MID, 0, pin_spacing);

    for( uint8_t i=1; i<CONFIG_JOLT_GUI_PIN_LEN; i++ ) {
        rollers[i] = lv_roller_create(pin_container, rollers[0]);
	    lv_roller_set_visible_row_count(rollers[i], 1);
	    lv_obj_align(rollers[i], rollers[i-1], LV_ALIGN_OUT_RIGHT_MID, 
                pin_spacing, 0);
    }
    lv_roller_set_action(rollers[CONFIG_JOLT_GUI_PIN_LEN-1], pin_entry_final_cb);

    lv_group_add_obj(g_main, pin_container);
    lv_group_focus_obj(rollers[0]);
    numerical_entry_loc = 0;
    
    lv_group_set_focus_cb(g_main, &update_selected_roller_style);

    jolt_gui_title_create(parent, "PIN (1/10)");
    MSG("PIN Container %p\n", pin_container);

    return LV_RES_OK;
}


