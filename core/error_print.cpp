#pragma once

#include "allocator.cpp"
#include "slice.cpp"

var error_print(AllocationError *) -> Slice<u8> {
    static char buf[] = "Allocation Error";
    return Slice<u8>{xx buf, sizeof(buf) - 1};
}
