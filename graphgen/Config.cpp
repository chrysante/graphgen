#include "Config.h"

using namespace graphgen;

static std::string& defFont() {
    static std::string font = "SF Mono";
    return font;
}

std::string graphgen::defaultFont() { return defFont(); }

void graphgen::setDefaultFont(std::string fontname) {
    defFont() = std::move(fontname);
}
