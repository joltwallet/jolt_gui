/* Provides placeholder functions when simulating gui not on device.
 */
#if PC_SIMULATOR
#include "../lvgl/lvgl.h"
#include "../lv_conf.h"
#include "stubs.h"

uint8_t get_battery_level() {
    /* Responds with a value between 0 and 100.
     * If a value above 100 is returned, the battery is charging.
     *
     * Cycles for demonstration purpose */
    static uint8_t level = 0;
    level = (level + 10) % 120;
    return level;
}
#endif
