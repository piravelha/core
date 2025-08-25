
#include "../core.cpp"

var entry() -> Result<void, AnyError> {
    var arena = ArenaAllocator{};
    try (init(&arena, page_allocator));
    defer (deinit(&arena));

    try (var foo in dupe(arena, Slice("foo")));
    defer (free(arena, foo));

    try (var bar in dupe(arena, Slice("bar")));
    defer (free(arena, bar));

    debug_print("{} {}\n", foo, bar);

    return {};
}


