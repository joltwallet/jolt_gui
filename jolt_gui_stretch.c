#include "jolt_gui_stretch.h"
#include "hal/storage/storage.h"


static void stretch_update_lv_task(jolt_key_stretch_t *stretch) {
    if(stretch->progress <= 100) {
        // The +10 makes it look better
        jolt_gui_scr_loading_update(stretch->scr, NULL, NULL,
                stretch->progress + 10);
    }
    else if( NULL != stretch->cb ) {
        /* Delete loading screen */
        lv_obj_del(stretch->scr);
        /* Delete self lv_task */
        lv_task_del(stretch->lv_task);
        /* Call complete callback */
        stretch->cb(NULL);
    }
}

void jolt_gui_stretch(const char *title, const char *label, uint8_t *key,
        lv_action_t complete_cb) {
    static jolt_key_stretch_t stretch;
    stretch.progress = 0;
    stretch.key = key;
    stretch.cb = complete_cb;
    stretch.scr = jolt_gui_scr_loading_create(title);
    jolt_gui_scr_loading_update(stretch.scr, NULL, label, 0);

    stretch.lv_task = lv_task_create(stretch_update_lv_task,
            100, LV_TASK_PRIO_HIGH,
            &stretch );

    // The stretch task is defined with storage because we (optionally) use
    // the ataes132a for hardware-bound encryption.
    xTaskCreate(storage_stretch_task,
            "PinStretch", 32000,
            (void *)&stretch,
            CONFIG_JOLT_TASK_PRIORITY_STRETCH, stretch.stretch_task);
}
