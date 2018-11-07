#if JOLT_GUI_DEBUG_FUNCTIONS

#include "lvgl/lvgl.h"
#include "esp_log.h"

static const char TAG[] = "gui_debug";

/* Prints out debug information about an object */
void jolt_gui_debug_obj_print(lv_obj_t *obj) {
    lv_obj_t *parent;
    lv_obj_type_t parent_type;

    parent = lv_obj_get_parent(obj);
	lv_obj_get_type(parent, &parent_type);

    lv_obj_type_t obj_type;
	lv_obj_get_type(obj, &obj_type);

    ESP_LOGI(TAG,
            "\n###################\n"
            "LVGL Object at %p\n"
            "Object Type: %s\n"
            "Parent at %p\n"
            "Parent Type: %s\n"
            "###################\n",
            obj, obj_type.type[0],
            parent, parent_type.type[0]
            );
}

#endif
