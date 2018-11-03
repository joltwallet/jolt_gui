#ifndef __JOLT_GUI_STRETCH__

#include "jolt_gui.h"

/* Returns immediately. Will call complete_cb from a lv_task upon completion */
void jolt_gui_stretch(const char *title, const char *label, uint8_t *key,
        lv_action_t complete_cb);

#endif
