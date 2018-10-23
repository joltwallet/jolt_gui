#ifndef __JOLT_GUI_QR_H__
#define __JOLT_GUI_QR_H__

#include "qrcode.h"

lv_img_dsc_t *jolt_gui_qr_to_img_dsc(QRCode *qrcode);
lv_obj_t *jolt_gui_qr_fullscreen_create(lv_img_dsc_t *qrcode_img,
        uint8_t scale, const char *title);

#endif
