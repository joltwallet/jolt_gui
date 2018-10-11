#include "../lv_misc/lv_font.h"

/***********************************************************
 * jolt_gui_symbols.ttf 9 px Font in U+F000  .. U+F000 range
************************************************************/

/*Store the image of the letters (glyph)*/
static const uint8_t jolt_gui_symbols_glyph_bitmap[] = 
{
    /* Unicode: U+F000, Width: 13 */
    0x00, 0x00,   //................
    0xFF, 0xF0,   //@@@@@@@@@@@@....
    0x80, 0x10,   //@..........@....
    0x80, 0x18,   //@..........@@...
    0x80, 0x18,   //@..........@@...
    0x80, 0x18,   //@..........@@...
    0x80, 0x10,   //@..........@....
    0xFF, 0xF0,   //@@@@@@@@@@@@....
    0x00, 0x00,   //................

    /* Unicode: U+F001, Width: 13 */
    0x00, 0x00,   //................
    0xFF, 0xF0,   //@@@@@@@@@@@@....
    0x80, 0x10,   //@..........@....
    0xB0, 0x18,   //@.@@.......@@...
    0xB0, 0x18,   //@.@@.......@@...
    0xB0, 0x18,   //@.@@.......@@...
    0x80, 0x10,   //@..........@....
    0xFF, 0xF0,   //@@@@@@@@@@@@....
    0x00, 0x00,   //................

    /* Unicode: U+F002, Width: 13 */
    0x00, 0x00,   //................
    0xFF, 0xF0,   //@@@@@@@@@@@@....
    0x80, 0x10,   //@..........@....
    0xB6, 0x18,   //@.@@.@@....@@...
    0xB6, 0x18,   //@.@@.@@....@@...
    0xB6, 0x18,   //@.@@.@@....@@...
    0x80, 0x10,   //@..........@....
    0xFF, 0xF0,   //@@@@@@@@@@@@....
    0x00, 0x00,   //................

    /* Unicode: U+F003, Width: 13 */
    0x00, 0x00,   //................
    0xFF, 0xF0,   //@@@@@@@@@@@@....
    0x80, 0x10,   //@..........@....
    0xB6, 0xD8,   //@.@@.@@.@@.@@...
    0xB6, 0xD8,   //@.@@.@@.@@.@@...
    0xB6, 0xD8,   //@.@@.@@.@@.@@...
    0x80, 0x10,   //@..........@....
    0xFF, 0xF0,   //@@@@@@@@@@@@....
    0x00, 0x00,   //................

    /* Unicode: U+F004, Width: 13 */
    0x02, 0x00,   //......@.........
    0xF6, 0xF0,   //@@@@.@@.@@@@....
    0x8C, 0x10,   //@...@@.....@....
    0x8E, 0x18,   //@...@@@....@@...
    0x9F, 0x98,   //@..@@@@@@..@@...
    0x87, 0x18,   //@....@@@...@@...
    0x83, 0x10,   //@.....@@...@....
    0xF6, 0xF0,   //@@@@.@@.@@@@....
    0x04, 0x00,   //.....@..........

    /* Unicode: U+F005, Width: 5 */
    0x20,   //..@..
    0x30,   //..@@.
    0xA8,   //@.@.@
    0x70,   //.@@@.
    0x20,   //..@..
    0x70,   //.@@@.
    0xA8,   //@.@.@
    0x30,   //..@@.
    0x20,   //..@..

    /* Unicode: U+F006, Width: 9 */
    0x00, 0x00,   //.........
    0x00, 0x00,   //.........
    0x00, 0x00,   //.........
    0x00, 0x00,   //.........
    0x00, 0x00,   //.........
    0x08, 0x00,   //....@....
    0x1C, 0x00,   //...@@@...
    0x08, 0x00,   //....@....
    0x00, 0x00,   //.........

    /* Unicode: U+F007, Width: 9 */
    0x00, 0x00,   //.........
    0x00, 0x00,   //.........
    0x00, 0x00,   //.........
    0x1C, 0x00,   //...@@@...
    0x22, 0x00,   //..@...@..
    0x49, 0x00,   //.@..@..@.
    0x1C, 0x00,   //...@@@...
    0x08, 0x00,   //....@....
    0x00, 0x00,   //.........
    
    /* Unicode: U+F008, Width: 9 */
    0x00, 0x00,   //.........
    0x3E, 0x00,   //..@@@@@..
    0x42, 0x00,   //.@.....@.
    0x9C, 0x80,   //@..@@@..@
    0x22, 0x00,   //..@...@..
    0x49, 0x00,   //.@..@..@.
    0x1C, 0x00,   //...@@@...
    0x08, 0x00,   //....@....
    0x00, 0x00,   //.........

    /* Unicode: U+F009, Width: 6 */
    0x30,   //..@@..
    0x48,   //.@..@.
    0x48,   //.@..@.
    0x48,   //.@..@.
    0xFC,   //@@@@@@
    0xFC,   //@@@@@@
    0xFC,   //@@@@@@
    0xFC,   //@@@@@@
    0x00,   //......
};

static const lv_font_glyph_dsc_t jolt_gui_symbols_dsc[] =
{
  {.w_px=13, .glyph_index=0}, /*Unicode: U++F000*/
  {.w_px=13, .glyph_index=18}, /*Unicode: U++F001*/
  {.w_px=13, .glyph_index=36}, /*Unicode: U++F002*/
  {.w_px=13, .glyph_index=54}, /*Unicode: U++F003*/
  {.w_px=13, .glyph_index=72}, /*Unicode: U++F004*/
  {.w_px=5, .glyph_index=90}, /*Unicode: U++F005*/
  {.w_px=9, .glyph_index=99}, /*Unicode: U++F006*/
  {.w_px=9, .glyph_index=117}, /*Unicode: U++F007*/
  {.w_px=9, .glyph_index=135}, /*Unicode: U++F008*/
  {.w_px=6, .glyph_index=153}, /*Unicode: U++F009*/
};

lv_font_t jolt_gui_symbols = 
{    
    .unicode_first = 0xF000,	/*First Unicode letter in this font*/
    .unicode_last = 0xF009,	/*Last Unicode letter in this font*/
    .h_px = 9,				/*Font height in pixels*/
    .glyph_bitmap = jolt_gui_symbols_glyph_bitmap,	/*Bitmap of glyphs*/
    .glyph_dsc = jolt_gui_symbols_dsc,		/*Description of glyphs*/
    .glyph_cnt = 10,			/*Number of glyphs in the font*/
    .unicode_list = NULL,	/*Every character in the font from 'unicode_first' to 'unicode_last'*/
    .get_bitmap = lv_font_get_bitmap_continuous,	/*Function pointer to get glyph's bitmap*/
    .get_width = lv_font_get_width_continuous,	/*Function pointer to get glyph's width*/
    .bpp = 1,				/*Bit per pixel*/
    .monospace = 0,				/*Fix width (0: if not used)*/
    .next_page = NULL,		/*Pointer to a font extension*/
};

