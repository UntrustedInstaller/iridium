#ifndef PIC_H
#define PIC_H

#define PIC_MASTER_CMD  0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CMD   0xA0
#define PIC_SLAVE_DATA  0xA1

#define PIC_ICW1_INIT    0x10
#define PIC_ICW1_ICW4    0x01
#define PIC_ICW4_8086    0x01

void pic_remap(void);
void pic_mask_all(void);

#endif
