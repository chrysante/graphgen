#include "graphgen/config.h"

using namespace graphgen;

static std::string& staticDefFont() {
    static std::string font = "SF Mono";
    return font;
}

std::string graphgen::defaultFont() { return staticDefFont(); }

void graphgen::defaultFont(std::string fontname) {
    staticDefFont() = std::move(fontname);
}
