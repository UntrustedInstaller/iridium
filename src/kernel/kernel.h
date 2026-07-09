#ifndef KERNEL_H
#define KERNEL_H

#include "io.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

enum vga_color {
    VGA_BLACK         = 0,
    VGA_BLUE          = 1,
    VGA_GREEN         = 2,
    VGA_CYAN          = 3,
    VGA_RED           = 4,
    VGA_MAGENTA       = 5,
    VGA_BROWN         = 6,
    VGA_LIGHT_GREY    = 7,
    VGA_DARK_GREY     = 8,
    VGA_LIGHT_BLUE    = 9,
    VGA_LIGHT_GREEN   = 10,
    VGA_LIGHT_CYAN    = 11,
    VGA_LIGHT_RED     = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN   = 14,
    VGA_WHITE         = 15,
};

#define vga_entry_color(fg, bg) ((uint8_t)((fg) | ((bg) << 4)))
#define vga_entry(uc, color)    ((uint16_t)((uint16_t)(uc) | ((uint16_t)(color) << 8)))

static inline int strlen(const char* s) {
    int n = 0;
    while (*s++) n++;
    return n;
}

static inline int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

static inline int strncmp(const char* a, const char* b, int n) {
    while (n-- && *a && *a == *b) { a++; b++; }
    if (n < 0) return 0;
    return *(unsigned char*)a - *(unsigned char*)b;
}

#include "io.h"

static inline void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x0C);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

static inline void serial_putchar(char c) {
    while ((inb(0x3F8 + 5) & 0x20) == 0);
    outb(0x3F8, c);
}

static inline void serial_write(const char* s) {
    while (*s) serial_putchar(*s++);
}

static inline void serial_write_dec(uint32_t val) {
    if (val == 0) { serial_putchar('0'); return; }
    char buf[12]; int p = 0;
    while (val > 0) { buf[p++] = '0' + val % 10; val /= 10; }
    while (p--) serial_putchar(buf[p]);
}

#endif
