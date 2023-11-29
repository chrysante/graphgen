#ifndef GRAPHGEN_STYLE_H_
#define GRAPHGEN_STYLE_H_

namespace graphgen {

/// List of colors
enum class Color { Black, White, Red, Green, Yellow, Blue, Magenta, Purple };

/// List of styles
enum class Style {
    Dashed,
    Dotted,
    Solid,
    Invisible,
    Bold,
};

} // namespace graphgen

#endif // GRAPHGEN_STYLE_H_
