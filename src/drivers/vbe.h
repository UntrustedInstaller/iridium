#ifndef VBE_H
#define VBE_H

#include "kernel.h"

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uint32_t pitch;
    uint32_t fb_addr;
    uint8_t  red_pos, red_mask;
    uint8_t  green_pos, green_mask;
    uint8_t  blue_pos, blue_mask;
    uint8_t  rsvd_pos, rsvd_mask;
} vbe_mode_info_t;

int vbe_init(uint16_t width, uint16_t height, uint8_t bpp);
void vbe_put_pixel(uint32_t x, uint32_t y, uint32_t color);
uint32_t vbe_make_color(uint8_t r, uint8_t g, uint8_t b);
void vbe_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void vbe_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void vbe_clear(uint32_t color);
void vbe_draw_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg);
void vbe_draw_string(uint32_t x, uint32_t y, const char* s, uint32_t fg, uint32_t bg);

uint32_t vbe_get_width(void);
uint32_t vbe_get_height(void);
uint32_t vbe_get_pitch(void);
uint32_t vbe_get_bpp(void);
uint32_t vbe_get_fb_addr(void);

#endif