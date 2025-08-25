#pragma once

#include "helpers.cpp"
#include "basic.cpp"

template <typename R>
struct Error {
    R fn;
    Error(R r) : fn(r) {}
};

template <>
struct Error<void> {
    Error() {}
};

Error() -> Error<void>;

template <typename T, typename R = void>
struct [[nodiscard]] Result {
    union {
        T value;
        R print;
    };

    bool ok;

    Result() {}
    Result(T t) {
        value = t;
        ok = true;
    }

    Result(Error<R> e) {
        print = e.fn;
        ok = false;
    }

    operator bool() { return ok; }

    Result<void, R> _to_void() { return *this; }
};

template <>
struct [[nodiscard]] Result<void, void> {
    static constexpr bool value = false;

    bool ok;

    Result() { ok = true; }

    template <typename T>
    Result(Result<T, void> r)
    {
        ok = r.ok;
    }

    template <typename T>
    operator Result<T, void>()
    {
        Result<T, void> r;
        r.ok = ok;
        return r;
    }

    comptime print = 0;

    Result(Error<void>) {
        ok = false;
    }

    operator bool() { return ok; }

    Result<void, void> _to_void() { return *this; }
};

template <typename T>
struct [[nodiscard]] Result<T, void> {
    T value;
    bool ok;

    Result() {}
    Result(T t) {
        value = t;
        ok = true;
    }

    comptime print = 0;

    Result(Error<void>) {
        ok = false;
    }

    operator bool() { return ok; }

    Result<void, void> _to_void() { return *this; }
};


template <typename R>
struct [[nodiscard]] Result<void, R> {
    union {
        R print;
    };
    static constexpr bool value = false;

    bool ok;

    Result() { ok = true; }

    template <typename T>
    Result(Result<T, R> r)
    {
        ok = r.ok;
        print = r.print;
    }

    template <typename T>
    operator Result<T, R>()
    {
        Result<T, R> r;
        r.ok = ok;
        r.print = print;
        return r;
    }

    Result(Error<R> e) {
        print = e.fn;
        ok = false;
    }

    operator bool() { return ok; }

    Result<void, R> _to_void() { return *this; }
};


