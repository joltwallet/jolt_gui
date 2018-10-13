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

static lv_group_focus_cb_t update_selected_roller_style(lv_obj_t *roller) {
    /* Only the focused roller has 3 visible selections;
     * all unselected rollers only have 1 */
    MSG("roller style callback!\n");
    if(jolt_gui_store.digit.pos != 0 ) {
	    lv_obj_align(jolt_gui_store.digit.rollers[0],
                NULL, LV_ALIGN_IN_LEFT_MID, jolt_gui_store.digit.offset, 0);
    }
    for(uint8_t i=0; i<CONFIG_JOLT_GUI_PIN_LEN; i++){
        uint8_t n_visible = 1;
        if( i == jolt_gui_store.digit.pos ) {
            n_visible = 3;
        }
        lv_roller_set_visible_row_count(jolt_gui_store.digit.rollers[i],
                n_visible);
        if( i > 0 ) {
	        lv_obj_align(jolt_gui_store.digit.rollers[i],
                    jolt_gui_store.digit.rollers[i-1],
                    LV_ALIGN_OUT_RIGHT_MID, 
                    jolt_gui_store.digit.spacing, 0);
        }
        else {
	        lv_obj_align(jolt_gui_store.digit.rollers[i], NULL,
                    LV_ALIGN_IN_LEFT_MID, jolt_gui_store.digit.offset, 0);
        }
    }
    return 0;
}


static lv_action_t digit_entry_cb(lv_obj_t *roller) {
    /* Callback used for all digit roller except the final */
    jolt_gui_store.digit.pos++;
    //lv_group_send_data(jolt_gui_store.group.main, LV_GROUP_KEY_NEXT);
    lv_group_focus_obj(
                jolt_gui_store.digit.rollers[jolt_gui_store.digit.pos]);
    return 0;
}


static lv_obj_t *digit_create(lv_obj_t *parent) {
    lv_obj_t *roller;
    roller = lv_roller_create(parent, NULL);
	lv_roller_set_visible_row_count(roller, 3);
    lv_roller_set_anim_time(roller, CONFIG_JOLT_GUI_ANIM_DIGIT_MS);
	lv_roller_set_options(roller, 
            "9\n8\n7\n6\n5\n4\n3\n2\n1\n0"
            "\n9\n8\n7\n6\n5\n4\n3\n2\n1\n0"
            "\n9\n8\n7\n6\n5\n4\n3\n2\n1\n0"
            "\n9\n8\n7\n6\n5\n4\n3\n2\n1\n0"
            );
    lv_roller_set_selected(roller, 19, false); // Set it to the middle 0 entry
    lv_roller_set_action(roller, digit_entry_cb);
    lv_group_add_obj(jolt_gui_store.group.main, roller);
    return roller;
}


lv_obj_t *jolt_gui_numeric_create( int8_t n, int8_t decimal, const char *title,
        lv_action_t cb ) { 
    lv_obj_t *parent = jolt_gui_parent_create();

    /* Create container to hold all rollers */
    lv_obj_t *cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, LV_HOR_RES, 
            LV_VER_RES - CONFIG_JOLT_GUI_STATUSBAR_H);
	lv_obj_align(cont, NULL, 
            LV_ALIGN_IN_TOP_LEFT, 0, CONFIG_JOLT_GUI_STATUSBAR_H);

    jolt_gui_store.digit.rollers[0] = digit_create(cont);

    /* Compute Roller Spacing */
    {
        uint8_t roller_width = lv_obj_get_width(jolt_gui_store.digit.rollers[0]);
        jolt_gui_store.digit.spacing = 
            ( lv_obj_get_width(cont) - n*roller_width ) / n;
        jolt_gui_store.digit.offset = ( lv_obj_get_width(cont) - 
                (n * (jolt_gui_store.digit.spacing+roller_width)) ) / 2;
    }

    /* Align first roller relative to container by offset */
	lv_obj_align(jolt_gui_store.digit.rollers[0], NULL,
            LV_ALIGN_IN_LEFT_MID, 0, jolt_gui_store.digit.offset);

    /* Create and align the remaining rollers */
    for( uint8_t i=1; i<n; i++ ) {
        jolt_gui_store.digit.rollers[i] = lv_roller_create(cont,
                jolt_gui_store.digit.rollers[0]);
	    lv_roller_set_visible_row_count(jolt_gui_store.digit.rollers[i], 1);
	    lv_obj_align(jolt_gui_store.digit.rollers[i],
                jolt_gui_store.digit.rollers[i-1], LV_ALIGN_OUT_RIGHT_MID, 
                jolt_gui_store.digit.spacing, 0);
    }

    /* Setup Callback on last roller */
    lv_roller_set_action(jolt_gui_store.digit.rollers[n-1], cb);

    /* Add rollers to group and focus on the first one */
    lv_group_add_obj(jolt_gui_store.group.main, cont);
    lv_group_focus_obj(jolt_gui_store.digit.rollers[0]);
    jolt_gui_store.digit.pos = 0;
    
    /* Set focus callback to change the number of visible options depending
     * on whether or not the roller is in focus */
    lv_group_set_focus_cb(jolt_gui_store.group.main, &update_selected_roller_style);

    /* Create Title Label */
    jolt_gui_title_create(parent, title);

    return parent;
}


