#include "Graph.h"

#include <iomanip>
#include <ostream>

using namespace graphgen;

static std::string& defFont() {
    static std::string font = "SF Mono";
    return font;
}

std::string graphgen::defaultFont() { return defFont(); }

void graphgen::setDefaultFont(std::string fontname) {
    defFont() = std::move(fontname);
}

static auto makeGenerator(std::string label) {
    return [label = std::move(label)](std::ostream& str) { str << label; };
}

Label::Label(std::string label, LabelKind kind):
    generator(makeGenerator(std::move(label))), _kind(kind) {}

Label::Label(std::function<void(std::ostream&)> generator, LabelKind kind):
    generator(std::move(generator)), _kind(kind) {}

void Label::emit(std::ostream& str) const {
    switch (kind()) {
    case LabelKind::PlainText:
        str << "\"";
        generator(str);
        str << "\"";
        break;

    case LabelKind::HTML:
        str << "<";
        generator(str);
        str << ">";
        break;
    }
}

Vertex::Vertex(ID id, Label label): _id(id), _label(std::move(label)) {
    setFont(defaultFont());
}
