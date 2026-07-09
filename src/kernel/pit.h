#ifndef PIT_H
#define PIT_H

#define PIT_CH0  0x40
#define PIT_CMD  0x43

#define PIT_FREQ 100

void pit_init(void);

#endif
