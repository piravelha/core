#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <assert.h>

#define var auto
#define comptime static constexpr auto
#define loop while (true)

#define Foreach_Helper(value) Foreach_Helper_<decltype(iterate(value)), decltype(next(declval(decltype(iterate(value)) *)))>

#define foreach3(iterator, it, value) \
    for (auto &&[_iterator, iterator, _it, _ok] = Foreach_Helper(value){iterate(value), {}, NULL, true}; \
        _it = next(&_iterator), _it; _ok = true) \
        for (auto &it = *_it; _ok; iterator = _iterator, _ok = false)

#define foreach2(it, value) foreach3(iterator, it, value)
#define foreach1(value) foreach3(iterator, it, value)
#define foreach_count_(_1, _2, _3, N, ...) N
#define foreach(...) foreach_count_(__VA_ARGS__, foreach3, foreach2, foreach1, _)(__VA_ARGS__)
#define in ,
#define it it
#define iterator iterator
#define err err

#define use1(U, _1) \
    auto &_1 = U._1
#define use2(U, _1, _2) \
    auto &_1 = U._1; \
    auto &_2 = U._2
#define use3(U, _1, _2, _3) \
    auto &_1 = U._1; \
    auto &_2 = U._2; \
    auto &_3 = U._3
#define use4(U, _1, _2, _3, _4) \
    auto &_1 = U._1; \
    auto &_2 = U._2; \
    auto &_3 = U._3; \
    auto &_4 = U._4 
#define use5(U, _1, _2, _3, _4, _5) \
    auto &_1 = U._1; \
    auto &_2 = U._2; \
    auto &_3 = U._3; \
    auto &_4 = U._4; \
    auto &_5 = U._5 

#define _count_use(_1, _2, _3, _4, _5, N, ...) N

#define _paste_(a, b) a##b
#define _paste(...) _paste_(__VA_ARGS__)
#define from(v) auto &&_paste(__use_, __LINE__) = v
#define use(...) ; _count_use(__VA_ARGS__, use5, use4, use3, use2, use1; _)(_paste(__use_, __LINE__), __VA_ARGS__)

#define _unpack1(U, _1) \
    auto _1 = get<0>(U);
#define _unpack2(U, _1, _2) \
    auto _1 = get<0>(U); \
    auto _2 = get<1>(U);
#define _unpack3(U, _1, _2, _3) \
    auto _1 = get<0>(U); \
    auto _2 = get<1>(U); \
    auto _3 = get<2>(U);
#define _unpack4(U, _1, _2, _3, _4) \
    auto _1 = get<0>(U); \
    auto _2 = get<1>(U); \
    auto _3 = get<2>(U); \
    auto _4 = get<3>(U);
#define _unpack5(U, _1, _2, _3, _4, _5) \
    auto _1 = get<0>(U); \
    auto _2 = get<1>(U); \
    auto _3 = get<2>(U); \
    auto _4 = get<3>(U); \
    auto _5 = get<4>(U);

#define unpack(...) ; _count_use(__VA_ARGS__, \
    _unpack5, \
    _unpack4, \
    _unpack3, \
    _unpack2, \
    _unpack1)(_paste(__use_, __LINE__), ##__VA_ARGS__)


#define try6(n, x, _1, _2, u, b) auto _paste(_try, __LINE__) = (x); if (!_paste(_try, __LINE__).ok) { auto u = _paste(_try, __LINE__).print; b; } n = _paste(_try, __LINE__).value
#define try5(x, _1, _2, u, b) try6(var _paste(__try_, __LINE__), x, , , u, b)
#define try4(n, x, _0, b) try6(n, x, , , err, b)
#define try3(x, _0, b) try6(var _paste(__try_, __LINE__), x, , , err, b)
#define try2(n, x) try6(n, x, , , err, return _paste(_try, __LINE__)._to_void())
#define try1(x) try6(var _paste(__try_, __LINE__), x, , , err, return _paste(_try, __LINE__)._to_void())
#define _count_try(_1, _2, _3, _4, _5, _6, N, ...) N
#define try(...) _count_try(__VA_ARGS__, try6, try5, try4, try3, try2, try1, _)(__VA_ARGS__)

#define catches(n) ,,, n,
#define catch ,,

#define _unwrap(v, r) assert((r).ok && "Unwrap failed"); v = (r).value
#define unwrap(...) _unwrap(__VA_ARGS__)

#define global_run(...) static struct _paste(__run_, __LINE__) { _paste(__run_, __LINE__)() { __VA_ARGS__; }} _paste(__run_val_, __LINE__){}


#define defer(...) auto _paste(__defer_, __LINE__) = _defer([&](){ __VA_ARGS__; })

#define derive_repr(T) void sprint(String *str, T t) { \
    sprint(str, #T); \
}

template <typename T>
struct _case_helper {
    T &&n;
    bool ok;
};

#define case3(u, T, n) auto _paste(_case, __LINE__) = Nullable<T>(get<T>(u)); if (_paste(_case, __LINE__)) for (auto &&[n, _i] = _case_helper<decltype(*_paste(_case, __LINE__))>{*_paste(_case, __LINE__), true}; _paste(_case, __LINE__) && _i; _i = false)
#define case2(u, T) case3(u, T, it)
#define case1(T) case3(__matcher, T, it)
#define _count_case(_1, _2, _3, N, ...) N
#define case(...) _count_case(__VA_ARGS__, case3, case2, case1)(__VA_ARGS__)
#define match(...) if (auto __matcher = (__VA_ARGS__); true)
#define as ,

#define xx __auto_cast_helper{} <<


template <typename T>
struct __auto_cast
{
    T t;

    template <typename R>
    operator R() { return (R)t; }
};

struct __auto_cast_helper
{
    template <typename T>
    __auto_cast<T> operator<<(T t) { __auto_cast<T> a{t}; return a; }
};



template <typename F>
struct _defer {
    F &&f;

    _defer(F &&_f) : f((F &&)_f) {}

    ~_defer()
    {
        f();
    }
};

template <typename F>
_defer(F &&) -> _defer<F>;

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

template <typename A, typename B>
struct EqualTypes {
    comptime value = false;
};

template <typename T>
struct EqualTypes<T, T> {
    comptime value = true;
};

template <typename Iterator, typename It>
struct Foreach_Helper_ {
    Iterator iterator;
    Iterator copy;
    It it;
    bool ok;
};


#include <stddef.h> // TODO

size_t _strlen(const char *s) {
    size_t i = 0;
    while (*s) {
        s++;
        i++;
    }
    return i;
}

bool _streq(const char *a, const char *b, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

long syscall(long number,
             long arg1, long arg2, long arg3,
             long arg4, long arg5, long arg6)
{
    register long x0 __asm__("x0") = arg1;
    register long x1 __asm__("x1") = arg2;
    register long x2 __asm__("x2") = arg3;
    register long x3 __asm__("x3") = arg4;
    register long x4 __asm__("x4") = arg5;
    register long x5 __asm__("x5") = arg6;
    register long x8 __asm__("x8") = number;

    __asm__ volatile(
        "svc #0"
        : "+r"(x0)        // result is returned in x0
        : "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x8)
        : "memory"
    );

    return x0;
}

#define SIGABRT 6


// TODO: argc, argv
var entry() -> Result<void>;


var main() -> int {
    try (entry() catch {
        assert(0 && "TODO: log error here\n");
    });

    exit(0);
    return 0;
}
