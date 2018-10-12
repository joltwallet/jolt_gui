#include "jolt_gui.h"
#include "jolt_gui_loading.h"

void jolt_gui_loading_update(uint8_t percentage, const char *text) {
	lv_bar_set_value_anim(jolt_gui_store.loading.bar, percentage,
            CONFIG_JOLT_GUI_LOADING_BAR_ANIM_MS);
    lv_label_set_text(jolt_gui_store.loading.label, text);
}

/* Create the loading object before initiating the task */
lv_obj_t *jolt_gui_loading_create(const char *title) {
    lv_obj_t *parent = jolt_gui_parent_create();

    /* Create Title */
    lv_obj_t *title_label = jolt_gui_title_create(parent, title);

    /* Create Container */
    lv_obj_t *cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, LV_HOR_RES, 
            LV_VER_RES - CONFIG_JOLT_GUI_STATUSBAR_H);
	lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_LEFT,
            0, CONFIG_JOLT_GUI_STATUSBAR_H);

    /* Create Loading Bar */
    jolt_gui_store.loading.bar = lv_bar_create(cont, NULL);
	lv_obj_set_size(jolt_gui_store.loading.bar, 
            CONFIG_JOLT_GUI_LOADING_BAR_W, CONFIG_JOLT_GUI_LOADING_BAR_H);
	lv_obj_align(jolt_gui_store.loading.bar, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
	lv_bar_set_value(jolt_gui_store.loading.bar, 1);

    lv_group_add_obj(jolt_gui_store.group.main, cont);
    lv_group_focus_obj(cont);

    /* Create Loading Label */
    jolt_gui_store.loading.label = lv_label_create(cont, NULL);
    lv_label_set_align(jolt_gui_store.loading.label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(jolt_gui_store.loading.label, "Initializing");
    lv_label_set_long_mode(jolt_gui_store.loading.label, LV_LABEL_LONG_ROLL);
    lv_style_t *label_style = lv_obj_get_style(jolt_gui_store.loading.label);
    lv_obj_set_size(jolt_gui_store.loading.label, CONFIG_JOLT_GUI_LOADING_TEXT_W,
            label_style->text.font->h_px);
	lv_obj_align(jolt_gui_store.loading.label, jolt_gui_store.loading.bar, 
            LV_ALIGN_OUT_TOP_MID, 0, -10);

    return parent;
}


