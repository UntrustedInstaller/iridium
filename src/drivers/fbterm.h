#ifndef FBTERM_H
#define FBTERM_H

#include "kernel.h"

#define FB_WIDTH  80
#define FB_HEIGHT 25

extern void fbterm_init(void);
extern void fbterm_putchar(char c);
extern void fbterm_write(const char* s);
extern void fbterm_clear(void);
extern void fbterm_setcolor(uint32_t fg, uint32_t bg);
extern void fbterm_scroll(void);
extern void fbterm_update_cursor(void);

#endif