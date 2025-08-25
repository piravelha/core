#pragma once

#include "helpers.cpp"
#include "allocator.cpp"

static struct DebugAllocator {
    operator Allocator() { return this; }
} debug_allocator;

var alloc(DebugAllocator *, usize size, usize) -> void * {
    return malloc(size);
}

var free(DebugAllocator *, void *ptr) -> void {
    free(ptr);
}
