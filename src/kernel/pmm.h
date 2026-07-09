#ifndef PMM_H
#define PMM_H

#include "kernel.h"

void pmm_init(void);
uint32_t pmm_alloc_frame(void) __attribute__((unused));
void pmm_free_frame(uint32_t frame) __attribute__((unused));
uint32_t pmm_get_free_frames(void) __attribute__((unused));
uint32_t pmm_get_used_frames(void) __attribute__((unused));

#endif