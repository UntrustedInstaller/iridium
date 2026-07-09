#include "kernel.h"
#include "terminal.h"

static uint16_t* const vga_buffer = (uint16_t*)0xB8000;

static uint16_t terminal_row = 0;
static uint16_t terminal_column = 0;
static uint8_t terminal_color;

static void terminal_putentryat(char c, uint8_t color, uint16_t x, uint16_t y) {
    vga_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

static void terminal_scroll(void) {
    for (uint16_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (uint16_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }
    for (uint16_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
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
        return;
    }
    if (c == '\r') {
        terminal_column = 0;
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
}

void terminal_initialize(void) {
    terminal_color = vga_entry_color(VGA_WHITE, VGA_BLUE);
    for (uint16_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = vga_entry(' ', terminal_color);
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_write(const char* data) {
    for (; *data; data++) terminal_putchar(*data);
}
