#ifndef __JOLT_GUI_STATUSBAR_H__
#define __JOLT_GUI_STATUSBAR_H__

#include "../../lv_conf.h"
#include "test_stubs.h"

enum {
    JOLT_GUI_STATUSBAR_INDEX_BATTERY = 0,
    JOLT_GUI_STATUSBAR_INDEX_WIFI,
    JOLT_GUI_STATUSBAR_INDEX_BLUETOOTH,
    JOLT_GUI_STATUSBAR_INDEX_LOCK,
};
#define JOLT_GUI_STATUSBAR_INDEX_NUM 4

#ifndef CONFIG_JOLT_GUI_STATUSBAR_H
    #define CONFIG_JOLT_GUI_STATUSBAR_H 12
#endif
#ifndef CONFIG_JOLT_GUI_TITLE_W
    #define CONFIG_JOLT_GUI_TITLE_W 70
#endif


void statusbar_create();

#endif
