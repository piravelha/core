#pragma once

#include "result.cpp"
#include "slice.cpp"
#include "allocator.cpp"
#include "error_print.cpp"

struct AnyError {
    void *data;
    Slice<u8> (*print_fn)(void *);

    AnyError() {}

    template <typename T>
    AnyError(T *t) {
        data = t;
        print_fn = +[](void *arg) {
            return error_print((T *)arg);
        };
    }

    template <typename A, typename T>
    AnyError(Result<A, T> *t) {
        data = t;
        print_fn = +[](void *arg) {
            return error_print((T *)arg);
        };
    }
};

var any_error_print(AnyError err) -> Slice<u8> {
    if (!err.data) {
        static char buf[] = "Error"; // TODO
        return Slice<u8>{xx buf, sizeof(buf) - 1};
    }
    return err.print_fn(err.data);
}

template <>
struct [[nodiscard]] Result<void, AnyError> {
    static constexpr bool value = false;

    AnyError print;

    bool ok;

    Result() {
        ok = true;
    }

    template <typename T, typename R>
    Result(Result<T, R> &&r)
    {
        ok = r.ok;
        print = {&r};
    }

    template <typename T>
    operator Result<T, AnyError>()
    {
        Result<T, AnyError> r;
        r.ok = ok;
        r.print = print;
        return r;
    }

    Result(Error<void>) {
        ok = false;
    }

    operator bool() { return ok; }

    Result<void, AnyError> _to_void() { return *this; }
};

template <typename T>
struct [[nodiscard]] Result<T, AnyError> {
    union {
        AnyError print;
        T value;
    };
    bool ok;

    Result() {}
    Result(T t) {
        value = t;
        ok = true;
    }

    Result(Error<AnyError> e) {
        print = e.fn;
        ok = false;
    }

    template <typename R>
    Result(Error<R> &&e) {
        print = {&e.fn};
        ok = false;
    }

    operator bool() { return ok; }

    Result<void, AnyError> _to_void() { return *this; }
};