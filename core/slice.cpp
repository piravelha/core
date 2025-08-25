#pragma once

#include "helpers.cpp"
#include "basic.cpp"

template <typename T>
struct Slice {
    T *data = NULL;
    usize count = 0;

    Slice() {}
    Slice(T *d, usize c) : data(d), count(c) {}

    Slice operator+(usize x) { return {data + x, count - x}; }

    T &operator[](usize i) { assert(i < count); return data[i]; }
    operator Slice<const T>() { return {data, count}; }
};

template <typename T>
struct Slice<const T> {
    T *data;
    usize count;

    Slice() {}
    Slice(T *d, usize c) : data(d), count(c) {}

    Slice operator+(usize x) { return {data + x, count - x}; }

    T &operator[](usize i) { assert(i < count); return data[i]; }
};

template <>
struct Slice<const u8> {
    const u8 *data = NULL;
    usize count = 0;

    Slice() {}
    Slice(const u8 *d, usize c) : data(d), count(c) {}
    Slice(const char *s) : data((const u8 *)s), count(_strlen(s)) {}
    Slice operator+(usize x) { return {data + x, count - x}; }

    const u8 &operator[](usize i) { assert(i < count); return data[i]; }
};

Slice(const char *) -> Slice<const u8>;
