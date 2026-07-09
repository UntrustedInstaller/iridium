#ifndef PIT_H
#define PIT_H

#include "kernel.h"

#define PIT_CH0  0x40
#define PIT_CMD  0x43

#define PIT_FREQ 100

void pit_init(void);
void pit_tick(void);
uint32_t pit_get_tick(void);

#endif
