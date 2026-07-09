#include "kernel.h"
#include "terminal.h"
#include "io.h"

static uint16_t* const vga_buffer = (uint16_t*)0xB8000;

static uint16_t terminal_row = 0;
static uint16_t terminal_column = 0;
static uint8_t terminal_color;

static void terminal_update_cursor(void) {
    uint16_t pos = terminal_row * VGA_WIDTH + terminal_column;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void terminal_putentryat(char c, uint8_t color, uint16_t x, uint16_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    vga_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

static void terminal_scroll(void) {
    // Move all lines up by one
    for (uint16_t y = 0; y < VGA_HEIGHT - 1; y++) {
        uint16_t* src = &vga_buffer[(y + 1) * VGA_WIDTH];
        uint16_t* dst = &vga_buffer[y * VGA_WIDTH];
        for (uint16_t x = 0; x < VGA_WIDTH; x++) {
            dst[x] = src[x];
        }
    }
    // Clear the last line with current color
    uint16_t blank = vga_entry(' ', terminal_color);
    uint16_t* last_line = &vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH];
    for (uint16_t x = 0; x < VGA_WIDTH; x++) {
        last_line[x] = blank;
    }
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            terminal_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
        terminal_update_cursor();
        return;
    }
    if (c == '\r') {
        terminal_column = 0;
        terminal_update_cursor();
        return;
    }
    if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
            terminal_update_cursor();
        }
        return;
    }
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            terminal_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
    }
    terminal_update_cursor();
}

void terminal_clear(void) {
    terminal_row = 0;
    terminal_column = 0;
    for (uint16_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = vga_entry(' ', terminal_color);
    }
}

void terminal_initialize(void) {
    // Set VGA text mode 80x25
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x0E);
    outb(0x3D4, 0x0B);
    outb(0x3D5, 0x0F);

    // Set custom blue palette entry 1
    outb(0x3C8, 1);
    outb(0x3C9, 22);
    outb(0x3C9, 11);
    outb(0x3C9, 32);

    terminal_color = vga_entry_color(VGA_WHITE, VGA_BLUE);
    terminal_clear();
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_write(const char* data) {
    for (; *data; data++) terminal_putchar(*data);
}