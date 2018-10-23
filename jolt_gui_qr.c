#include "jolt_gui.h"

static const uint8_t color_header[] = {
    0x04, 0x02, 0x04, 0xff,     /*Color of index 0*/
    0xff, 0xff, 0xff, 0xff,     /*Color of index 1*/
};

/* Will malloc the space. QRCode doesn't need to exist after this call*/
lv_img_dsc_t *jolt_gui_qr_to_img_dsc(QRCode *qrcode) {
    uint8_t byte_width = qrcode->size / 8; // Number of bytes to store a row
    if( qrcode->size & 0x07 ) { // ceil to nearest byte
        byte_width++;
    }
	uint16_t data_size = (qrcode->size)*byte_width + 8;
    uint8_t *data = calloc(data_size, sizeof(uint8_t));
    memcpy(data, color_header, sizeof(color_header));
    uint8_t *img = data + sizeof(color_header);

    for(uint8_t y=0; y < qrcode->size; y++){ // iterate through rows
        for(uint8_t x=0; x < qrcode->size; x++){ // iterate through cols
            if(!qrcode_getModule(qrcode, x, y)){
                uint8_t pos = y*byte_width + x/8;
                img[pos] |= (1 << (7 - (x & 0x07)) );
            }
        }
    }

    lv_img_dsc_t *qrcode_img = lv_mem_alloc(sizeof(lv_img_dsc_t));
    qrcode_img->header.always_zero = 0;
    qrcode_img->header.w = qrcode->size;
    qrcode_img->header.h = qrcode->size;
    qrcode_img->data_size = data_size;
    qrcode_img->header.cf = LV_IMG_CF_INDEXED_1BIT;
    qrcode_img->data = data;

    return qrcode_img;
}

static lv_action_t delete_screen(lv_obj_t *btn) {
    // free memory use by img
    lv_obj_t *img = lv_obj_get_parent(btn);
    lv_img_ext_t *ext = lv_obj_get_ext_attr(img);
    uint8_t *data = ((lv_img_dsc_t *)(ext->src))->data;

    lv_img_set_src( img, NULL );
    free(data);

    jolt_gui_delete_current_screen();
    return 0;
}

lv_obj_t *jolt_gui_qr_fullscreen_create(lv_img_dsc_t *qrcode_img, 
        uint8_t scale, const char *title) {
    lv_obj_t *parent = jolt_gui_parent_create();
    lv_obj_align(parent, jolt_gui_store.statusbar.container, 
            LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    lv_obj_t *cont = lv_cont_create(parent, NULL);
    lv_obj_set_size(cont, LV_HOR_RES, LV_VER_RES-CONFIG_JOLT_GUI_STATUSBAR_H);

    lv_obj_t *img = lv_img_create(cont, NULL);
    lv_img_set_src( img, qrcode_img );
    lv_img_set_auto_size(img, true);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_group_add_obj(jolt_gui_store.group.main, parent);
    lv_group_focus_obj(parent);

    jolt_gui_set_enter_action(parent, NULL);

    /* Set img as parent of back action so that we can easily get img
     * attributes while deleting */
    jolt_gui_set_back_action(img, delete_screen);

    jolt_gui_title_create(parent, title);

    return parent;
}
