#ifndef GRAPHGEN_STREAMMANIP_H_
#define GRAPHGEN_STREAMMANIP_H_

#include <concepts>
#include <type_traits>
#include <iosfwd>

namespace graphgen {

template <typename F>
class StreamManip {
public:
    constexpr StreamManip() requires std::is_default_constructible_v<F> = default;
    constexpr StreamManip(F const& f): function(f) {}
    constexpr StreamManip(F&& f): function(std::move(f)) {}
    
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const {
        return graphgen::StreamManip([=, f = function](std::ostream& ostream) { std::invoke(f, ostream, args...); });
    }
    
    template <typename CharT, typename Traits>
    requires std::invocable<F, std::basic_ostream<CharT, Traits>&>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& ostream, StreamManip<F> const& manip) {
        std::invoke(manip.function, ostream);
        return ostream;
    }
    
private:
    F function;
};

} // namespace graphgen

#endif // GRAPHGEN_STREAMMANIP_H_
