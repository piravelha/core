#pragma once


#include "helpers.cpp"
#include "basic.cpp"
#include "debug_print.cpp"
#include "mem.cpp"
#include "allocator.cpp"

#define NAI_HEAP_SIZE (1024 * 4096)

var align_up(usize n, usize align) -> usize {
    return (n + align - 1) & ~(align - 1);
}

struct PageAllocator {
    struct Block {
        usize size;
        bool free;
        Block *next;
    };

    u8 *heap = NULL;
    Block *free_list = NULL;

    operator Allocator() { return this; }
};



var init(PageAllocator *a) -> void {
    from (*a) use (heap, free_list);
    using Block = PageAllocator::Block;

    heap = (u8 *)mmap(NULL, NAI_HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (!heap) {
        debug_panic("Failed to allocate heap");
    }

    free_list = (Block *)heap;
    free_list->size = NAI_HEAP_SIZE - sizeof(Block);
    free_list->free = true;
    free_list->next = NULL;
}

var deinit(PageAllocator *a) -> void {
    from (*a) use (heap, free_list);

    if (!heap) {
        debug_panic("Attempting to deinit with null heap");
    }

    munmap(heap, NAI_HEAP_SIZE);
    mem_set(a, 0);
}

var alloc(PageAllocator *a, usize size, usize align) -> void * {
    from (*a) use (heap, free_list);
    using Block = PageAllocator::Block;

    size = align_up(size, align);
    Block *curr = free_list;

    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size >= size + sizeof(Block) + 8) {
                Block *new_block = (Block *)((unsigned char *)curr + sizeof(Block) + size);

                new_block->size = curr->size - size - sizeof(Block);
                new_block->free = true;
                new_block->next = curr->next;

                curr->size = size;
                curr->next = new_block;
            }

            curr->free = false;
            return (uint8_t *)curr + sizeof(Block);
        }

        curr = curr->next;
    }

    return NULL;
}

var free(PageAllocator *a, void *ptr) -> void {
    from (*a) use (heap, free_list);
    using Block = PageAllocator::Block;

    Block *block = (Block *)((u8 *)ptr - sizeof(Block));
    block->free = true;

    if (block->next && block->next->free) {
        block->size += sizeof(Block) + block->next->size;
        block->next = block->next->next;
    }
}

var dump(PageAllocator a) -> void {
    from (a) use (heap, free_list);
    using Block = PageAllocator::Block;

    Block *block = a.free_list;
    debug_print("[");
    while (block) {
        debug_print("{}, ", block->size);
        block = block->next;
    }
    debug_print("(null)]\n");
}

static PageAllocator page_allocator;
global_run(init(&page_allocator));
