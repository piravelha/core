#pragma once

#include "helpers.cpp"
#include "result.cpp"
#include "debug_print.cpp"
#include "any_error.cpp"

// TODO: argc, argv
var entry() -> Result<void, AnyError>;


var main() -> int {
    try (entry() catch {
        var output = any_error_print(err);
        debug_print("Program returned error: {}\n", output);
        debug_abort();

        return 1;
    });

    return 0;
}
