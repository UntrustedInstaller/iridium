#include "pit.h"
#include "io.h"

volatile uint32_t tick = 0;

void pit_init(void) {
    uint32_t divisor = 1193180 / PIT_FREQ;

    outb(PIT_CMD, 0x36);
    io_wait();
    outb(PIT_CH0, divisor & 0xFF);
    io_wait();
    outb(PIT_CH0, (divisor >> 8) & 0xFF);
}
