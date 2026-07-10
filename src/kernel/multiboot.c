#include "multiboot.h"
#include "kernel.h"

#define MAX_MMAP_ENTRIES 32

static multiboot_mmap_entry_t memory_map[MAX_MMAP_ENTRIES];
static int mmap_entry_count = 0;

static uint32_t total_memory_kb = 0;
static uint32_t lower_memory_kb = 0;
static uint32_t upper_memory_kb = 0;

void multiboot_init(uint32_t magic, uint32_t addr) {
    outb(0x3F8, 'M');
    if (magic != 0x2BADB002 && magic != 0x2BADB045) {
        return;
    }
    outb(0x3F8, 'm');

    struct multiboot_header* mbi = (struct multiboot_header*)addr;
    outb(0x3F8, 'f');

    if (!(mbi->flags & MULTIBOOT_FLAG_MEM)) {
        return;
    }
    outb(0x3F8, 'e');

    lower_memory_kb = mbi->mem_lower;
    upper_memory_kb = mbi->mem_upper;
    total_memory_kb = lower_memory_kb + upper_memory_kb;
    outb(0x3F8, 'M');

if (mbi->flags & MULTIBOOT_FLAG_MMAP) {
        outb(0x3F8, 'p');
        uint32_t mmap_addr = mbi->mmap_addr;
        uint32_t mmap_length = mbi->mmap_length;
        outb(0x3F8, '0' + ((mmap_addr >> 28) & 0xF));
        outb(0x3F8, '0' + ((mmap_addr >> 24) & 0xF));
        outb(0x3F8, '0' + ((mmap_addr >> 20) & 0xF));
        outb(0x3F8, '0' + ((mmap_addr >> 16) & 0xF));
        outb(0x3F8, '0' + ((mmap_addr >> 12) & 0xF));
        outb(0x3F8, '0' + ((mmap_addr >> 8) & 0xF));
        outb(0x3F8, '0' + ((mmap_addr >> 4) & 0xF));
        outb(0x3F8, '0' + (mmap_addr & 0xF));
        outb(0x3F8, ' ');
        
        multiboot_mmap_entry_t* entry = (multiboot_mmap_entry_t*)mbi->mmap_addr;
        outb(0x3F8, 'l');
        int iter = 0;
        
        while ((uint32_t)entry < (mbi->mmap_addr + mmap_length) && mmap_entry_count < MAX_MMAP_ENTRIES) {
            outb(0x3F8, '0' + (iter % 10));
            outb(0x3F8, 'c');
            memory_map[mmap_entry_count++] = *entry;
            outb(0x3F8, 'c');
            outb(0x3F8, 's');
            outb(0x3F8, '0' + (entry->size % 10));
            outb(0x3F8, 'n');
            uint32_t next = (uint32_t)entry + entry->size + sizeof(entry->size);
            if (next >= mbi->mmap_addr + mmap_length || next < (uint32_t)entry) {
                outb(0x3F8, 'E');
                break;
            }
            outb(0x3F8, 'n');
            entry = (multiboot_mmap_entry_t*)next;
            outb(0x3F8, 'N');
            if (++mmap_entry_count > 20) break;
        }
        outb(0x3F8, 'L');
    }
    outb(0x3F8, 'X');
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