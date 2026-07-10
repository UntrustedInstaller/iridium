#include "paging.h"
#include "pmm.h"
#include "kernel.h"

static uint32_t* page_directory = 0;

void paging_init(void) {
    page_directory = (uint32_t*)0x2000;
    for (int i = 0; i < PAGE_DIR_ENTRIES; i++) {
        page_directory[i] = 0;
    }

    uint32_t* pt0 = (uint32_t*)0x3000;
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        pt0[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITE;
    }
    page_directory[0] = ((uint32_t)pt0) | PAGE_PRESENT | PAGE_WRITE;

    uint32_t fb_virt = 0xFD000000;
    uint32_t fb_pde_idx = fb_virt >> 22;
    uint32_t* pt_fb = (uint32_t*)0x4000;
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        pt_fb[i] = (fb_virt + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITE;
    }
    page_directory[fb_pde_idx] = ((uint32_t)pt_fb) | PAGE_PRESENT | PAGE_WRITE;

    uint32_t heap_virt = 0xE0000000;
    uint32_t heap_pde_idx = heap_virt >> 22;
    uint32_t* pt_heap = (uint32_t*)0x5000;
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        pt_heap[i] = 0;
    }
    page_directory[heap_pde_idx] = ((uint32_t)pt_heap) | PAGE_PRESENT | PAGE_WRITE;

    __asm__ volatile("mov %0, %%cr3" : : "r"((uint32_t)page_directory));

    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

void paging_map_page(uint32_t virt, uint32_t phys, uint32_t flags) {
    uint32_t pde_idx = virt >> 22;
    uint32_t pte_idx = (virt >> 12) & 0x3FF;

    uint32_t pde = page_directory[pde_idx];
    if (!(pde & PAGE_PRESENT)) {
        return;
    }

    uint32_t* pt = (uint32_t*)(pde & 0xFFFFF000);
    pt[pte_idx] = (phys & 0xFFFFF000) | (flags & 0xFFF) | PAGE_PRESENT;
    __asm__ volatile("invlpg (%0)" : : "r"(virt));
}

void paging_unmap_page(uint32_t virt) {
    uint32_t pde_idx = virt >> 22;
    uint32_t pte_idx = (virt >> 12) & 0x3FF;

    uint32_t pde = page_directory[pde_idx];
    if (!(pde & PAGE_PRESENT)) return;

    uint32_t* pt = (uint32_t*)(pde & 0xFFFFF000);
    pt[pte_idx] = 0;
    __asm__ volatile("invlpg (%0)" : : "r"(virt));
}

uint32_t paging_virt_to_phys(uint32_t virt) {
    uint32_t pde_idx = virt >> 22;
    uint32_t pte_idx = (virt >> 12) & 0x3FF;

    uint32_t pde = page_directory[pde_idx];
    if (!(pde & PAGE_PRESENT)) return 0;

    uint32_t* pt = (uint32_t*)(pde & 0xFFFFF000);
    uint32_t pte = pt[pte_idx];
    if (!(pte & PAGE_PRESENT)) return 0;

    return (pte & 0xFFFFF000) + (virt & 0xFFF);
}
