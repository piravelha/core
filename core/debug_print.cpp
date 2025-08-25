#pragma once

#include "helpers.cpp"
#include "debug_allocator.cpp"
#include "print.cpp"

template <typename... Args>
var debug_print(const char *fmt, Args... args) -> void {
    var result = alloc_print({&debug_allocator}, fmt, args...);
    
    if (!result) {
        debug_panic("Alloc failed");
    }

    unwrap (var buf in result);
    defer (free(&debug_allocator, buf));

#ifdef DEBUG
#ifdef _WIN32
    DWORD written;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(h, buf.data, (DWORD)buf.count, &written, NULL);
#else
    write(1, buf.data, buf.count);
#endif
#endif
}

