#include "gdt.h"

static struct gdt_entry gdt_entries[3];

static void gdt_set_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].limit_low   = limit & 0xFFFF;
    gdt_entries[num].base_low    = base & 0xFFFF;
    gdt_entries[num].base_mid    = (base >> 16) & 0xFF;
    gdt_entries[num].access      = access;
    gdt_entries[num].granularity = gran | ((limit >> 16) & 0x0F);
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;
}

void gdt_init(void) {
    gdt_set_entry(0, 0, 0, 0, 0);

    gdt_set_entry(1, 0, 0xFFFFFFFF,
        0x9A,  // present, ring 0, code, non-conforming, readable
        0xCF); // 4K granularity, 32-bit

    gdt_set_entry(2, 0, 0xFFFFFFFF,
        0x92,  // present, ring 0, data, writable
        0xCF);

    struct gdt_ptr gp;
    gp.limit = sizeof(gdt_entries) - 1;
    gp.base  = (uint32_t)&gdt_entries;

    gdt_flush((uint32_t)&gp);
}
