#ifndef PAGING_H
#define PAGING_H

#include "kernel.h"

#define PAGE_PRESENT  (1 << 0)
#define PAGE_WRITE    (1 << 1)
#define PAGE_USER     (1 << 2)
#define PAGE_ACCESSED (1 << 5)
#define PAGE_DIRTY    (1 << 6)
#define PAGE_GLOBAL   (1 << 8)

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES 1024
#define PAGE_DIR_ENTRIES 1024

void paging_init(void);
void paging_map_page(uint32_t virt, uint32_t phys, uint32_t flags);
void paging_unmap_page(uint32_t virt);
uint32_t paging_virt_to_phys(uint32_t virt);

#endif
