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

uint8_t get_bluetooth_level() {
    /* Returns with the bluetooth strength level. 0 if no connected. */
    static uint8_t level = 0;
    level = !level;
    return level;
}

uint8_t get_wifi_level() {
    /* Returns with the wifi strength level */
    static uint8_t i = 0;
    uint8_t levels[] = {0, 50, 70, 100};
    i = (i + 1) % sizeof(levels);
    return levels[i];
}

uint8_t get_lock_status() {
    static uint8_t level = 0;
    level = !level;
    return level;
}

#endif
