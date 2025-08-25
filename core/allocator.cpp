#pragma once

#include "helpers.cpp"
#include "mem.cpp"


struct Allocator {
    void *data;
    void *(*alloc_fn)(void *, usize size, usize align);
    void (*free_fn)(void *, void *ptr);

    Allocator() {}

    template <typename A>
    Allocator(A *a) {
        data = a;
        alloc_fn = +[](void *arg, usize size, usize align) {
            return alloc((A *)arg, size, align);
        };
        free_fn = +[](void *arg, void *ptr) {
            free((A *)arg, ptr);
        };
    }
};

template <typename T>
var alloc(Allocator a, usize count = 1) -> Result<Slice<T>> {
    from (a) use (alloc_fn, data);

    var ptr = alloc_fn(data, sizeof(T) * count, alignof(T));
    if (!ptr) return {};
    return Slice<T>{(T *)ptr, count};
}

template <typename T>
var free(Allocator a, Slice<T> ptr) -> void {
    from (a) use (free_fn, data);
    
    free_fn(data, (void *)ptr.data);
}


template <typename T>
var dupe(Allocator a, Slice<const T> value) -> Result<Slice<T>> {
    try (var copy in alloc<T>(a, value.count));
    mem_copy(xx copy.data, xx value.data, sizeof(T) * value.count);
    return copy;
}
