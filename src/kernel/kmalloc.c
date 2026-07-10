#include "kmalloc.h"
#include "paging.h"
#include "pmm.h"
#include "kernel.h"

typedef struct heap_block {
    uint32_t magic;
    uint32_t size;
    struct heap_block* next;
} heap_block_t;

#define HEAP_MAGIC_FREE 0xDEADBEEF
#define HEAP_MAGIC_USED 0xCAFEBABE
#define HEAP_ALIGN 8
#define HEADER_SIZE ((sizeof(heap_block_t) + HEAP_ALIGN - 1) & ~(HEAP_ALIGN - 1))

static heap_block_t* free_list = 0;
static uint32_t heap_cur = HEAP_VIRT_BASE;
static uint32_t heap_max = HEAP_VIRT_BASE;

static void heap_extend(uint32_t size) {
    uint32_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t i = 0; i < pages; i++) {
        uint32_t phys = pmm_alloc_frame();
        if (!phys) return;
        paging_map_page(heap_max, phys, PAGE_WRITE);
        heap_max += PAGE_SIZE;
    }
}

void kmalloc_init(void) {
    heap_extend(HEAP_INITIAL_SIZE);
    heap_cur = HEAP_VIRT_BASE + HEADER_SIZE;
}

void* kmalloc(uint32_t size) {
    if (size == 0) return 0;

    uint32_t aligned = (size + HEAP_ALIGN - 1) & ~(HEAP_ALIGN - 1);
    uint32_t needed = HEADER_SIZE + aligned;

    heap_block_t* prev = 0;
    heap_block_t* block = free_list;

    while (block) {
        if (block->magic == HEAP_MAGIC_FREE && block->size >= needed) {
            if (block->size >= needed + HEADER_SIZE + HEAP_ALIGN) {
                heap_block_t* new_block = (heap_block_t*)((uint32_t)block + needed);
                new_block->magic = HEAP_MAGIC_FREE;
                new_block->size = block->size - needed;
                new_block->next = block->next;
                if (prev) prev->next = new_block;
                else free_list = new_block;
                block->size = needed;
            } else {
                if (prev) prev->next = block->next;
                else free_list = block->next;
            }
            block->magic = HEAP_MAGIC_USED;
            return (void*)((uint32_t)block + HEADER_SIZE);
        }
        prev = block;
        block = block->next;
    }

    heap_extend(needed);
    heap_block_t* new_block = (heap_block_t*)heap_cur;
    new_block->magic = HEAP_MAGIC_USED;
    new_block->size = needed;
    new_block->next = 0;
    void* ptr = (void*)(heap_cur + HEADER_SIZE);
    heap_cur += needed;
    return ptr;
}

void kfree(void* ptr) {
    if (!ptr) return;
    heap_block_t* block = (heap_block_t*)((uint32_t)ptr - HEADER_SIZE);
    if (block->magic != HEAP_MAGIC_USED) return;

    block->magic = HEAP_MAGIC_FREE;
    block->next = free_list;
    free_list = block;

    heap_block_t* cur = free_list;
    while (cur && cur->next) {
        if ((uint32_t)cur + cur->size == (uint32_t)cur->next) {
            cur->size += cur->next->size;
            cur->next = cur->next->next;
            continue;
        }
        cur = cur->next;
    }
}
