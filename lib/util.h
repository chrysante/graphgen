#ifndef GRAPHGEN_UTIL_H_
#define GRAPHGEN_UTIL_H_

#include <concepts>
#include <iosfwd>
#include <type_traits>

namespace graphgen {

[[noreturn]] inline void unreachable() {
#if defined(__GNUC__) // GCC, Clang, ICC
    __builtin_unreachable();
#elif defined(_MSC_VER) // MSVC
    __assume(false);
#endif
}

template <typename F>
struct ScopeGuard {
    constexpr ScopeGuard(F f): function(f) {}

    ScopeGuard(ScopeGuard&&) = delete;

    constexpr ~ScopeGuard() { std::invoke(function); }

    F function;
};

template <typename T>
struct ObjWrapper {
    T obj;
};

template <typename T>
struct ObjWrapper<T&> {
    T& obj;
};

template <typename T>
ObjWrapper(T&&) -> ObjWrapper<T>;

template <typename T>
ObjWrapper(T&) -> ObjWrapper<T&>;

template <typename F>
class StreamManip {
public:
    constexpr StreamManip() requires std::is_default_constructible_v<F>
    = default;
    constexpr StreamManip(F const& f): function(f) {}
    constexpr StreamManip(F&& f): function(std::move(f)) {}

    template <typename... Args>
    constexpr auto operator()(Args&&... args) const {
        return graphgen::StreamManip(
            [args = std::tuple{ ObjWrapper{ std::forward<Args>(args) }... },
             f = function](std::ostream& ostream) {
            std::apply(
                [&](auto&... args) { std::invoke(f, ostream, args.obj...); },
                args);
        });
    }

    template <typename CharT, typename Traits>
    requires std::invocable < F, std::basic_ostream<CharT, Traits>
    & > friend std::basic_ostream<CharT, Traits>& operator<<(
            std::basic_ostream<CharT, Traits>& ostream,
            StreamManip<F> const& manip) {
        std::invoke(manip.function, ostream);
        return ostream;
    }

private:
    F function;
};

} // namespace graphgen

#endif // GRAPHGEN_UTIL_H_
