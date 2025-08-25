#pragma once

#include "helpers.cpp"
#include "slice.cpp"
#include "allocator.cpp"
#include "debug_panic.cpp"
#include "mem.cpp"

// TODO: strlit instead of const char *?

var alloc_print_string(Allocator a, Slice<const u8> s) -> Result<Slice<u8>, AllocationError> {
    try (var buf in alloc<u8>(a, s.count));
    mem_copy(xx buf.data, xx s.data, s.count);

    return buf;
}

var alloc_print_unsigned_int(Allocator a, u64 n) -> Result<Slice<u8>, AllocationError> {
    usize i = 0;

    var len = usize(1);
    var copy = n;
    while (copy >= 10) {
        copy /= 10;
        ++len;
    }

    try (var buf in alloc<u8>(a, len));

    if (n == 0) {
        buf[i] = '0';
        return buf;
    }

    while (n > 0) {
        var j = i++;
        buf[len - j - 1] = '0' + u8(n % 10);
        n /= 10;
    }
    
    return buf;
}

var alloc_print_signed_int(Allocator a, s64 n) -> Result<Slice<u8>, AllocationError> {
    usize i = 0;
    usize len = 0;

    bool is_negative = false;
    if (n < 0) {
        ++len;
        ++i;
        n = -n;
        is_negative = true;
    }

    var copy = n;
    while (copy > 0) {
        copy /= 10;
        ++len;
    }

    try (var buf in alloc<u8>(a, len));

    if (n == 0) {
        buf[i] = '0';
        return buf;
    }

    if (is_negative) {
        buf[0] = '-';
    }

    while (n > 0) {
        var j = i++;
        buf[len - j - 1 + usize(is_negative)] = '0' + u8(n % 10);
        n /= 10;
    }
    
    return buf;
}

var alloc_print(Allocator a, const char *fmt) -> Result<Slice<u8>, AllocationError> {
    return alloc_print_string(a, {fmt});
}

template <typename First, typename... Rest>
var alloc_print(Allocator a, const char *fmt, First first, Rest... rest) -> Result<Slice<u8>, AllocationError> {
    var begin = fmt;
    var end = fmt;

    while (*fmt) {
        var buf = Slice<u8>{};

        if (_streq(fmt, "{}", 2)) {
            if constexpr (
                EqualTypes<First, char *>::value ||
                EqualTypes<First, const char *>::value ||
                EqualTypes<First, Slice<u8>>::value ||
                EqualTypes<First, Slice<const u8>>::value)
            {
                try (buf in alloc_print_string(a, Slice<const u8>{first}));
            } else if constexpr (
                EqualTypes<First, u8>::value ||
                EqualTypes<First, u16>::value ||
                EqualTypes<First, u32>::value ||
                EqualTypes<First, u64>::value ||
                EqualTypes<First, usize>::value)
            {
                try (buf in alloc_print_unsigned_int(a, u64(first)));
            } else if constexpr (
                EqualTypes<First, s8>::value ||
                EqualTypes<First, s16>::value ||
                EqualTypes<First, s32>::value ||
                EqualTypes<First, s64>::value)
            {
                try (buf in alloc_print_signed_int(a, s64(first)));
            } else if constexpr (
                EqualTypes<First, bool>::value)
            {
                if (first) {
                    try (buf in alloc<u8>(a, 4));
                    mem_copy(buf.data, xx "true", 4);
                } else {
                    try (buf in alloc<u8>(a, 5));
                    mem_copy(buf.data, xx "false", 5);
                }
            } else {
                static_assert(0 && "Type not supported");
            }

            end = fmt;
            fmt += 2;
        } else {
            fmt++;
            continue;
        }

        var len = usize(end - begin);

        defer (free(a, buf));

        try (var result in alloc_print(a, fmt, rest...));
        defer (free(a, result));

        try (var combined in alloc<u8>(a, len + buf.count + result.count));

        mem_copy(combined.data, xx begin, len);
        mem_copy(combined.data + len, buf.data, buf.count);
        mem_copy(combined.data + len + buf.count, result.data, result.count);

        return combined;
    }

    debug_panic("Missing format argument");
    return {};
}



