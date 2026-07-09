#include "multiboot.h"
#include "terminal.h"

struct multiboot_header* multiboot_info = 0;

void multiboot_init(uint32_t magic, uint32_t addr) {
    if (magic != 0x2BADB002) {
        terminal_write("Invalid multiboot magic!\n");
        return;
    }
    multiboot_info = (struct multiboot_header*)addr;
}

uint32_t multiboot_get_total_memory_kb(void) {
    if (!multiboot_info) return 0;
    return multiboot_info->mem_lower + multiboot_info->mem_upper;
}

uint32_t multiboot_get_lower_memory_kb(void) {
    if (!multiboot_info) return 0;
    return multiboot_info->mem_lower;
}

uint32_t multiboot_get_upper_memory_kb(void) {
    if (!multiboot_info) return 0;
    return multiboot_info->mem_upper;
}

int multiboot_has_mmap(void) {
    if (!multiboot_info) return 0;
    return multiboot_info->flags & MULTIBOOT_FLAG_MMAP;
}