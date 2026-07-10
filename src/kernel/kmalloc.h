#ifndef KMALLOC_H
#define KMALLOC_H

#include "kernel.h"

#define HEAP_VIRT_BASE 0xE0000000
#define HEAP_INITIAL_SIZE 0x4000

void kmalloc_init(void);
void* kmalloc(uint32_t size);
void kfree(void* ptr);

#endif
