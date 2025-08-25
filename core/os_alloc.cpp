#pragma once


#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/syscall.h>
    #include <sys/mman.h>
#endif

#include <stdlib.h>

static inline void *os_alloc(size_t size) {
#ifdef _WIN32
    return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return ptr == MAP_FAILED ? NULL : ptr;
#endif
}

static inline void os_free(void *ptr, size_t size) {
#ifdef _WIN32
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}
