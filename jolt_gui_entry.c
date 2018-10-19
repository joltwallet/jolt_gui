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
static lv_action_t back_cb(lv_obj_t *btn);

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
    if(jolt_gui_store.digit.pos != 0 ) {
        lv_obj_align(jolt_gui_store.digit.rollers[0],
                NULL, LV_ALIGN_IN_LEFT_MID, jolt_gui_store.digit.offset, 0);
    }
    for(uint8_t i=0; i<jolt_gui_store.digit.n; i++){
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
    //lv_group_focus_obj(
    //            jolt_gui_store.digit.rollers[jolt_gui_store.digit.pos]);
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
    //lv_group_add_obj(jolt_gui_store.group.main, roller);
    return roller;
}

#if 0
lv_obj_t *jolt_gui_numeric_create( int8_t n, int8_t decimal, const char *title,
        lv_action_t cb ) { 

    jolt_gui_store.digit.decimal = decimal;
    jolt_gui_store.digit.n = n;

    lv_obj_t *parent = jolt_gui_parent_create();

    /* Create container to hold all rollers */
    lv_obj_t *cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, LV_HOR_RES, 
            LV_VER_RES - CONFIG_JOLT_GUI_STATUSBAR_H);
    lv_obj_align(cont, NULL, 
            LV_ALIGN_IN_TOP_LEFT, 0, CONFIG_JOLT_GUI_STATUSBAR_H);

    jolt_gui_store.digit.rollers[0] = digit_create(cont);

    /* Create decimal point obj so its behind rollers */
    lv_obj_t *dp = NULL;
    if( jolt_gui_store.digit.decimal >= 0 ) {
        dp = lv_label_create(cont, NULL);
    }

    /* Compute Roller Spacing */
    {
        uint8_t roller_width = lv_obj_get_width(jolt_gui_store.digit.rollers[0]);
        jolt_gui_store.digit.spacing = 
            ( lv_obj_get_width(cont) - n*roller_width ) / n;
        jolt_gui_store.digit.offset = ( lv_obj_get_width(cont) - 
                (n-1) * jolt_gui_store.digit.spacing -
                n * roller_width ) / 2;
        printf("offset: %d\n", jolt_gui_store.digit.offset);
        printf("spacing: %d\n", jolt_gui_store.digit.spacing);
        printf("width: %d\n", roller_width);
    }

    /* Align first roller relative to container by offset */
    lv_obj_align(jolt_gui_store.digit.rollers[0], NULL,
            LV_ALIGN_IN_LEFT_MID, jolt_gui_store.digit.offset, 0);

    /* Create and align the remaining rollers */
    for( uint8_t i=1; i<n; i++ ) {
        jolt_gui_store.digit.rollers[i] = lv_roller_create(cont,
                jolt_gui_store.digit.rollers[0]);
        lv_roller_set_visible_row_count(jolt_gui_store.digit.rollers[i], 1);
        lv_obj_align(jolt_gui_store.digit.rollers[i],
                jolt_gui_store.digit.rollers[i-1], LV_ALIGN_OUT_RIGHT_MID, 
                jolt_gui_store.digit.spacing, 0);
    }

    /* Configure decimal point */
    if( jolt_gui_store.digit.decimal >= 0 ) {
        lv_label_set_style(dp, lv_roller_get_style(jolt_gui_store.digit.rollers[0], LV_ROLLER_STYLE_BG));
        lv_label_set_static_text(dp, ".");
        if( jolt_gui_store.digit.decimal == n) {
            lv_obj_align(dp, jolt_gui_store.digit.rollers[0],
                    LV_ALIGN_OUT_LEFT_MID, 0,0);
        }
        else {
            lv_obj_align(dp, jolt_gui_store.digit.rollers[
                    n-jolt_gui_store.digit.decimal-1],
                    LV_ALIGN_OUT_RIGHT_MID, 0,0);
        }
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

    /* Set Back Action (delete pin screen) */
    jolt_gui_set_back_action(parent, &back_cb);

    return parent;
}
#endif

static lv_action_t back_cb(lv_obj_t *btn) {
    if( jolt_gui_store.digit.pos <= 0 ) {
        /* Close the current container */
        jolt_gui_delete_current_screen();
    }
    else {
        /* Preserve the currently selected roller value */
        lv_roller_ext_t *ext = lv_obj_get_ext_attr(
                jolt_gui_store.digit.rollers[jolt_gui_store.digit.pos]);
        ext->ddlist.sel_opt_id_ori = ext->ddlist.sel_opt_id;

        /* Decrement position and refocus */
        jolt_gui_store.digit.pos--;
        MSG("Decrementing jolt_gui_store.digit.pos to %d\n",
                jolt_gui_store.digit.pos);
        lv_group_focus_obj(jolt_gui_store.digit.rollers[
                jolt_gui_store.digit.pos]);
    }
    return 0;
}




/**********************
 * STATIC VARIABLES
 ***********************/
static lv_res_t jolt_gui_num_signal(lv_obj_t *btn, lv_signal_t sign, void *param);
static lv_signal_func_t ancestor_signal;


/* new api */
lv_obj_t *jolt_gui_num_create(lv_obj_t * par, const lv_obj_t * copy) {
    LV_LOG_TRACE("list create started");

    /*Create the ancestor basic object*/
    lv_obj_t *new_num = lv_cont_create(par, NULL);
    lv_mem_assert(new_num);
    if(new_num == NULL) return NULL;
    lv_obj_set_size(new_num, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(new_num, 0, 0);

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_num);

    /* Declare and initialize ext */
    jolt_gui_num_ext_t *ext = lv_obj_allocate_ext_attr(new_num,
            sizeof(jolt_gui_num_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;
    ext->n = 0;
    ext->pos = 0;
    ext->decimal = -1;
    ext->spacing = 0;
    ext->offset = 0;
    memset(ext->rollers, 0, CONFIG_JOLT_GUI_NUMERIC_LEN * sizeof(lv_obj_t *));

    lv_obj_set_signal_func(new_num, jolt_gui_num_signal);

    /* Create decimal point obj so its behind rollers */
    ext->decimal_obj = lv_label_create(new_num, NULL);
    lv_label_set_static_text(ext->decimal_obj, ".");
    lv_obj_set_hidden(ext->decimal_obj, true); // hide the decimal point

    //jolt_gui_set_enter_action(new_num, 
    //
    /*
    if( NULL != ext->rollers[0] ) {
        lv_obj_del( ext->rollers[0] );
    }*/
    ext->rollers[0] = digit_create(new_num);

    /*Init the new list object*/
    if(copy == NULL) {
    }
    else {
        // todo
    }
    return new_num;
}

void jolt_gui_num_set_num_digits(lv_obj_t *num, uint8_t n) {
    if( 0 == n || n > CONFIG_JOLT_GUI_NUMERIC_LEN) {
        return;
    }
    jolt_gui_num_ext_t *ext = lv_obj_get_ext_attr(num);

    ext->n = n; // update number of rollers


    /* copy the roller style for the decimal */
    lv_label_set_style( ext->decimal_obj,
            lv_roller_get_style(ext->rollers[0], LV_ROLLER_STYLE_BG) );

    /* Compute Roller Spacing */
    {
        uint8_t roller_width = lv_obj_get_width(ext->rollers[0]);
        ext->spacing = ( lv_obj_get_width(num) - ext->n*roller_width ) / ext->n;
        ext->offset = ( lv_obj_get_width(num) - 
                (ext->n-1) * ext->spacing -
                ext->n * roller_width ) / 2;
        printf("offset: %d\n", ext->offset);
        printf("spacing: %d\n", ext->spacing);
        printf("width: %d\n", roller_width);
    }

    /* Align first roller relative to container by offset */
    lv_obj_align(ext->rollers[0], NULL,
            LV_ALIGN_IN_LEFT_MID, ext->offset, 0);

    for(uint8_t i=1; i < ext->n; i++) {
        if( NULL != ext->rollers[i] ) {
            lv_obj_del(ext->rollers[i]);
        }

        ext->rollers[i] = lv_roller_create(num, ext->rollers[0]);
        lv_roller_set_visible_row_count(ext->rollers[i], 1);
        lv_obj_align(ext->rollers[i],
                ext->rollers[i-1], LV_ALIGN_OUT_RIGHT_MID, 
                ext->spacing, 0);
    }

    //lv_group_focus_obj(ext->rollers[0]);
}

void jolt_gui_num_set_decimal(lv_obj_t *num, uint8_t pos) {
    jolt_gui_num_ext_t *ext = lv_obj_get_ext_attr(num);
    ext->decimal = pos;
    /* Align the decimal point */
    if( pos > 0 ) {
        // Different alignment policy if its the left-most roller
        if( ext->decimal == ext->n) {
            lv_obj_align(ext->decimal_obj, ext->rollers[0],
                    LV_ALIGN_OUT_LEFT_MID, 0,0);
        }
        else {
            lv_obj_align(ext->decimal_obj, ext->rollers[ext->decimal-1],
                    LV_ALIGN_OUT_RIGHT_MID, 0,0);
        }
        lv_obj_set_hidden(ext->decimal_obj, false); // unhide the decimal point
    }
    else {
        lv_obj_set_hidden(ext->decimal_obj, true); // hide the decimal point
    }
}

void jolt_gui_num_set_action(lv_obj_t *num, lv_action_t cb){
    /* Setup Callback on last roller */
    jolt_gui_num_ext_t *ext = lv_obj_get_ext_attr(num);
    lv_roller_set_action(ext->rollers[ext->n-1], cb);
}

static lv_res_t jolt_gui_num_signal(lv_obj_t *btn, lv_signal_t sign, void *param) {
    printf("LV_SIGNAL_RELEASED const %d\n", LV_SIGNAL_RELEASED);
    printf("doge %d\n", sign);
    lv_res_t res;

    /* Include the ancestor signal function */
    res = ancestor_signal(btn, sign, param);
    if(res != LV_RES_OK) return res;

#if USE_LV_GROUP
    if(sign == LV_SIGNAL_CONTROLL){
        char c = *((char *)param);
        switch(c){
            case LV_GROUP_KEY_DOWN:
                printf("Pressed Down\n");
                break;
            case LV_GROUP_KEY_UP:
                printf("Pressed up\n");
                break;
            case LV_GROUP_KEY_ENTER:
                printf("Pressed enter\n");
                break;
            default:
                break;
        }
        if(sign == LV_SIGNAL_RELEASED) {
            lv_obj_t *num = lv_obj_get_parent(lv_obj_get_parent(btn));
            lv_group_t *g = lv_obj_get_group(num);
            if(lv_group_get_focused(g) == num 
                    && lv_indev_is_dragging(lv_indev_get_act()) == false) {
                printf("meow\n");
                // todo
            }
        }
    }
#endif

    return res;
}
