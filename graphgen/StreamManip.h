#ifndef GRAPHGEN_STREAMMANIP_H_
#define GRAPHGEN_STREAMMANIP_H_

#include <concepts>
#include <iosfwd>
#include <type_traits>

namespace graphgen {

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
    constexpr StreamManip()
        requires std::is_default_constructible_v<F>
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
        requires std::invocable<F, std::basic_ostream<CharT, Traits>&>
    friend std::basic_ostream<CharT, Traits>& operator<<(
        std::basic_ostream<CharT, Traits>& ostream,
        StreamManip<F> const& manip) {
        std::invoke(manip.function, ostream);
        return ostream;
    }

private:
    F function;
};

} // namespace graphgen

#endif // GRAPHGEN_STREAMMANIP_H_
