#pragma once

#include "helpers.cpp"
#include "basic.cpp"

var mem_copy(u8 *buf, const u8 *mem, usize len) -> void {
    for (usize i = 0; i < len; ++i) {
        buf[i] = mem[i];
    }
}

template <typename T>
var mem_set(T *value, u8 byte) -> void {
    var buf = (u8 *)value;

    for (usize i = 0; i < sizeof(T); ++i) {
        buf[i] = byte;
    }
}

var align_up(usize n, usize align) -> usize {
    return (n + align - 1) & ~(align - 1);
}