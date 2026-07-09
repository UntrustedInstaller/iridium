#ifndef TERMINAL_H
#define TERMINAL_H

#include "kernel.h"

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putchar(char c);
void terminal_write(const char* data);
void terminal_clear(void);

#endif
