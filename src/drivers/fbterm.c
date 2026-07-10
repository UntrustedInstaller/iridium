#include "fbterm.h"
#include "vbe.h"
#include "kernel.h"

static uint32_t term_fg = 0xFFFFFFFF;
static uint32_t term_bg = 0xFF7845A8;
static uint32_t term_cursor_color = 0xFF9B6FD0;
static uint32_t term_x = 0;
static uint32_t term_y = 0;
static int cursor_visible = 0;

static void fbterm_draw_cursor(void) {
    for (uint32_t col = 0; col < 8; col++) {
        for (uint32_t row = 0; row < 16; row++) {
            vbe_put_pixel(term_x + col, term_y + row, term_cursor_color);
        }
    }
    cursor_visible = 1;
}

static void fbterm_erase_cursor(void) {
    if (!cursor_visible) return;
    for (uint32_t col = 0; col < 8; col++) {
        for (uint32_t row = 0; row < 16; row++) {
            vbe_put_pixel(term_x + col, term_y + row, term_bg);
        }
    }
    cursor_visible = 0;
}

void fbterm_init(void) {
    fbterm_clear();
    cursor_visible = 0;
    term_x = 0;
    term_y = 0;
}

void fbterm_putchar(char c) {
    fbterm_erase_cursor();

    if (c == '\n') {
        term_x = 0;
        term_y += 16;
        if (term_y >= vbe_get_height() - 16) {
            fbterm_scroll();
        }
    } else if (c == '\r') {
        term_x = 0;
    } else if (c == '\b') {
        if (term_x >= 8) {
            term_x -= 8;
            vbe_draw_char(term_x, term_y, ' ', term_fg, term_bg);
        }
    } else if (c >= 0x20 && c <= 0x7E) {
        if (term_x >= vbe_get_width() - 8) {
            term_x = 0;
            term_y += 16;
            if (term_y >= vbe_get_height() - 16) {
                fbterm_scroll();
            }
        }
        vbe_draw_char(term_x, term_y, c, term_fg, term_bg);
        term_x += 8;
    }

    fbterm_draw_cursor();
}

void fbterm_write(const char* s) {
    while (*s) {
        fbterm_putchar(*s++);
    }
}

void fbterm_clear(void) {
    cursor_visible = 0;
    vbe_clear(term_bg);
    term_x = 0;
    term_y = 0;
}

void fbterm_setcolor(uint32_t fg, uint32_t bg) {
    term_fg = fg;
    term_bg = bg;
}

void fbterm_scroll(void) {
    uint32_t h = vbe_get_height();
    uint32_t w = vbe_get_width();
    uint32_t pitch = vbe_get_pitch();
    uint32_t fb = vbe_get_fb_addr();

    for (uint32_t y = 16; y < h; y++) {
        uint32_t* src = (uint32_t*)(fb + y * pitch);
        uint32_t* dst = (uint32_t*)(fb + (y - 16) * pitch);
        for (uint32_t x = 0; x < w; x++) {
            dst[x] = src[x];
        }
    }
    for (uint32_t y = h - 16; y < h; y++) {
        uint32_t* row = (uint32_t*)(fb + y * pitch);
        for (uint32_t x = 0; x < w; x++) {
            row[x] = term_bg;
        }
    }
    term_y -= 16;
}

void fbterm_update_cursor(void) {
    if (cursor_visible) {
        fbterm_erase_cursor();
    } else {
        fbterm_draw_cursor();
    }
}