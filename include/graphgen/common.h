#ifndef GRAPHGEN_COMMON_H_
#define GRAPHGEN_COMMON_H_

namespace graphgen {

[[noreturn]] inline void unreachable() {
#if defined(__GNUC__)
    __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(false);
#endif
}

} // namespace graphgen

#endif // GRAPHGEN_COMMON_H_
