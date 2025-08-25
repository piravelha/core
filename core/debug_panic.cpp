#pragma once


__attribute__((noreturn))
var debug_panic_(const char *file, int line, const char *msg) -> void {
#ifdef DEBUG
    printf("DEBUG PANIC at %s:%d: %s\n", file, line, msg);
    abort();
#endif
}

#define debug_panic(msg) debug_panic_(__FILE__, __LINE__, msg)
