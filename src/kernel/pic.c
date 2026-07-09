#include "pic.h"
#include "io.h"

void pic_remap(void) {
    uint8_t a1 = inb(PIC_MASTER_DATA);
    uint8_t a2 = inb(PIC_SLAVE_DATA);

    outb(PIC_MASTER_CMD,  PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    outb(PIC_SLAVE_CMD,   PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    outb(PIC_MASTER_DATA, 0x20);
    io_wait();
    outb(PIC_SLAVE_DATA,  0x28);
    io_wait();
    outb(PIC_MASTER_DATA, 4);
    io_wait();
    outb(PIC_SLAVE_DATA,  2);
    io_wait();
    outb(PIC_MASTER_DATA, PIC_ICW4_8086);
    io_wait();
    outb(PIC_SLAVE_DATA,  PIC_ICW4_8086);
    io_wait();

    outb(PIC_MASTER_DATA, a1);
    outb(PIC_SLAVE_DATA,  a2);
}

void pic_mask_all(void) {
    outb(PIC_MASTER_DATA, 0xFF);
    outb(PIC_SLAVE_DATA,  0xFF);
}
