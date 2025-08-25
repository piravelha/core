#pragma once

#include "helpers.cpp"
#include "debug_allocator.cpp"
#include "print.cpp"

template <typename... Args>
var debug_print(const char *fmt, Args... args) -> void {
    var result = alloc_print({&debug_allocator}, fmt, args...);
    assert(result && "Alloc failed");

    unwrap (var buf in result);
    defer (free(&debug_allocator, buf));

    write(1, buf.data, buf.count);
}

