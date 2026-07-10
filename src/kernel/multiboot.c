#include "multiboot.h"
#include "kernel.h"

#define MAX_MMAP_ENTRIES 32

static multiboot_mmap_entry_t memory_map[MAX_MMAP_ENTRIES];
static int mmap_entry_count = 0;

static uint32_t total_memory_kb = 0;
static uint32_t lower_memory_kb = 0;
static uint32_t upper_memory_kb = 0;

void multiboot_init(uint32_t magic, uint32_t addr) {
    if (magic != 0x2BADB002 && magic != 0x2BADB045) {
        return;
    }

    struct multiboot_header* mbi = (struct multiboot_header*)addr;

    if (!(mbi->flags & MULTIBOOT_FLAG_MEM)) {
        return;
    }

    lower_memory_kb = mbi->mem_lower;
    upper_memory_kb = mbi->mem_upper;
    total_memory_kb = lower_memory_kb + upper_memory_kb;

    if (mbi->flags & MULTIBOOT_FLAG_MMAP) {
        if (mbi->mmap_addr >= 0x100000 && mbi->mmap_length > 0 && mbi->mmap_length < 0x10000) {
            multiboot_mmap_entry_t* entry = (multiboot_mmap_entry_t*)mbi->mmap_addr;
            while ((uint32_t)entry < (mbi->mmap_addr + mbi->mmap_length) && mmap_entry_count < MAX_MMAP_ENTRIES) {
                memory_map[mmap_entry_count++] = *entry;
                uint32_t next = (uint32_t)entry + entry->size + sizeof(entry->size);
                if (next >= mbi->mmap_addr + mbi->mmap_length || next < (uint32_t)entry || entry->size == 0) {
                    break;
                }
                entry = (multiboot_mmap_entry_t*)next;
            }
        }
    }
}

__attribute__((unused)) uint32_t multiboot_get_total_memory_kb(void) {
    return total_memory_kb;
}

__attribute__((unused)) uint32_t multiboot_get_lower_memory_kb(void) {
    return lower_memory_kb;
}

__attribute__((unused)) uint32_t multiboot_get_upper_memory_kb(void) {
    return upper_memory_kb;
}

__attribute__((unused)) int multiboot_has_mmap(void) {
    return mmap_entry_count > 0;
}

__attribute__((unused)) int multiboot_get_mmap_count(void) {
    return mmap_entry_count;
}

__attribute__((unused)) multiboot_mmap_entry_t* multiboot_get_mmap_entry(int idx) {
    if (idx >= 0 && idx < mmap_entry_count) {
        return &memory_map[idx];
    }
    return 0;
}