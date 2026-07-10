#include "pmm.h"
#include "multiboot.h"
#include "terminal.h"
#include "kernel.h"

#define PAGE_SIZE 4096

static uint32_t* frame_bitmap = 0;
static uint32_t total_frames = 0;
static uint32_t used_frames = 0;

static inline void bitmap_set(uint32_t bit) {
    frame_bitmap[bit / 32] |= (1 << (bit % 32));
}

static inline void bitmap_clear(uint32_t bit) {
    frame_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static inline int bitmap_test(uint32_t bit) {
    return frame_bitmap[bit / 32] & (1 << (bit % 32));
}

static uint32_t find_free_frame(void) {
    for (uint32_t i = 0; i < total_frames / 32; i++) {
        if (frame_bitmap[i] != 0xFFFFFFFF) {
            for (uint32_t b = 0; b < 32; b++) {
                if (!(frame_bitmap[i] & (1 << b))) {
                    return i * 32 + b;
                }
            }
        }
    }
    return 0xFFFFFFFF;
}

void pmm_init(void) {
    total_frames = multiboot_get_total_memory_kb() * 1024 / PAGE_SIZE;

    // Place bitmap after kernel
    extern uint32_t _kernel_end;
    frame_bitmap = (uint32_t*)(((uint32_t)&_kernel_end + 3) & ~3);
    uint32_t total_mem = multiboot_get_total_memory_kb() * 1024;
    if ((uint32_t)frame_bitmap + ((total_frames + 7) / 8) > (uint32_t)total_mem) {
        // Fallback to fixed address
        frame_bitmap = (uint32_t*)0x100000;
    }

    // All frames used by default
    uint32_t bitmap_ints = (total_frames + 31) / 32;
    for (uint32_t i = 0; i < bitmap_ints; i++) {
        frame_bitmap[i] = 0xFFFFFFFF;
    }
    used_frames = total_frames;

    int mmap_count = multiboot_get_mmap_count();
    if (mmap_count > 0) {
        // Walk mmap: free only what BIOS says is available
        for (int i = 0; i < mmap_count; i++) {
            multiboot_mmap_entry_t* entry = multiboot_get_mmap_entry(i);
            if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                uint32_t start = entry->base_addr_low;
                uint32_t length = entry->length_low;
                uint32_t end = start + length;
                uint32_t first_frame = (start + PAGE_SIZE - 1) / PAGE_SIZE;
                uint32_t last_frame = end / PAGE_SIZE;
                if (first_frame < last_frame) {
                    for (uint32_t f = first_frame; f < last_frame; f++) {
                        if (f < total_frames) {
                            bitmap_clear(f);
                            used_frames--;
                        }
                    }
                }
            }
        }
    } else {
        // Synthetic memory map from mem_lower/mem_upper
        uint32_t lower_kb = multiboot_get_lower_memory_kb();
        uint32_t upper_kb = multiboot_get_upper_memory_kb();
        if (lower_kb == 0 && upper_kb == 0) {
            lower_kb = 640;
            upper_kb = 64512;
        }

        // Lower memory: 0 - 640KB (skip frame 0)
        uint32_t last_frame = (lower_kb * 1024) / PAGE_SIZE;
        for (uint32_t f = 1; f < last_frame; f++) {
            if (f < total_frames) { bitmap_clear(f); used_frames--; }
        }

        // Upper memory: 1MB onwards
        uint32_t upper_start = 0x100000;
        uint32_t upper_end = upper_start + upper_kb * 1024;
        uint32_t first_upper = upper_start / PAGE_SIZE;
        uint32_t last_upper = upper_end / PAGE_SIZE;
        for (uint32_t f = first_upper; f < last_upper; f++) {
            if (f < total_frames) { bitmap_clear(f); used_frames--; }
        }
    }

    // Now re-reserve specific regions the kernel needs.
    // The mmap marked kernel RAM as AVAILABLE, so we must explicitly
    // reserve kernel image, bitmap, and frame 0.

    // Reserve frame 0 (IVT/BDA)
    if (!bitmap_test(0)) {
        bitmap_set(0);
        used_frames++;
    }

    // Reserve kernel image frames (loaded at 0x100000 through _kernel_end)
    extern uint32_t _kernel_end;
    uint32_t kernel_start = 0x100000;
    uint32_t kernel_end = (uint32_t)&_kernel_end;
    uint32_t kernel_first = kernel_start / PAGE_SIZE;
    uint32_t kernel_last = (kernel_end + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t f = kernel_first; f < kernel_last; f++) {
        if (f < total_frames && !bitmap_test(f)) {
            bitmap_set(f);
            used_frames++;
        }
    }

    // Reserve bitmap frames (placed right after _kernel_end)
    uint32_t bitmap_addr = (uint32_t)frame_bitmap;
    uint32_t bitmap_size = (total_frames + 7) / 8;
    uint32_t bitmap_first = bitmap_addr / PAGE_SIZE;
    uint32_t bitmap_last = (bitmap_addr + bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t f = bitmap_first; f < bitmap_last; f++) {
        if (f < total_frames && !bitmap_test(f)) {
            bitmap_set(f);
            used_frames++;
        }
    }
}

uint32_t pmm_alloc_frame(void) {
    uint32_t frame = find_free_frame();
    if (frame != 0xFFFFFFFF) {
        bitmap_set(frame);
        used_frames++;
        return frame * PAGE_SIZE;
    }
    return 0;
}

void pmm_free_frame(uint32_t frame) {
    if (frame == 0) return;
    uint32_t idx = frame / PAGE_SIZE;
    if (idx < total_frames) {
        bitmap_clear(idx);
        used_frames--;
    }
}

uint32_t pmm_get_free_frames(void) {
    (void)total_frames;
    (void)used_frames;
    return total_frames - used_frames;
}

uint32_t pmm_get_used_frames(void) {
    (void)used_frames;
    return used_frames;
}