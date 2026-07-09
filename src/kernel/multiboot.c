#include "multiboot.h"
#include "terminal.h"
#include "kernel.h"

#define MAX_MMAP_ENTRIES 32

static multiboot_mmap_entry_t memory_map[MAX_MMAP_ENTRIES];
static int mmap_entry_count = 0;

static uint32_t total_memory_kb = 0;
static uint32_t lower_memory_kb = 0;
static uint32_t upper_memory_kb = 0;

void multiboot_init(uint32_t magic, uint32_t addr) {
    if (magic != 0x2BADB002 && magic != 0x2BADB045) {
        terminal_write("Multiboot: invalid magic!\n");
        return;
    }

    struct multiboot_header* mbi = (struct multiboot_header*)addr;

    if (!(mbi->flags & MULTIBOOT_FLAG_MEM)) {
        terminal_write("Multiboot: no mem info\n");
        return;
    }

    lower_memory_kb = mbi->mem_lower;
    upper_memory_kb = mbi->mem_upper;
    total_memory_kb = lower_memory_kb + upper_memory_kb;

    if (mbi->flags & MULTIBOOT_FLAG_MMAP) {
    }

    }

uint32_t multiboot_get_total_memory_kb(void) {
    return total_memory_kb;
}

uint32_t multiboot_get_lower_memory_kb(void) {
    return lower_memory_kb;
}

uint32_t multiboot_get_upper_memory_kb(void) {
    return upper_memory_kb;
}

int multiboot_has_mmap(void) {
    return mmap_entry_count > 0;
}

int multiboot_get_mmap_count(void) {
    return mmap_entry_count;
}

multiboot_mmap_entry_t* multiboot_get_mmap_entry(int idx) {
    if (idx >= 0 && idx < mmap_entry_count) {
        return &memory_map[idx];
    }
    return 0;
}