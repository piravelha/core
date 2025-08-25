#pragma once

#include "helpers.cpp"
#include "allocator.cpp"
#include "mem.cpp"

#define ARENA_SIZE (8 * 1024)

struct ArenaAllocator {
    Allocator base;
    Slice<u8> memory = {};
    usize used = 0;

    operator Allocator() { return this; }
};

var init(ArenaAllocator *a, Allocator base) -> Result<void, AllocationError> {
    from (*a) use (memory, used);

    a->base = base;
    try (memory in alloc<u8>(base, ARENA_SIZE));
    used = 0;

    return {};
}

var deinit(ArenaAllocator *a) {
    from (*a) use (base, memory);
    free(base, memory);
}

void *alloc(ArenaAllocator *a, usize size, usize align) {
    from (*a) use (base, memory, used);
    var aligned = align_up(used, align);

    if (aligned + size > memory.count) {
        return NULL;
    }

    used = aligned;

    var old = used;
    used += size;
    return memory.data + old;
}

void free(ArenaAllocator *, void *) {}
