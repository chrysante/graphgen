#include "Graph.h"

#include <iomanip>
#include <ostream>

#include "Config.h"
#include "VertexVisitor.h"

using namespace graphgen;

static auto makeGenerator(std::string text) {
    return [text = std::move(text)](std::ostream& str) { str << text; };
}

Label::Label(std::string text, LabelKind kind):
    generator(makeGenerator(std::move(text))), _kind(kind) {}

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

Vertex::Vertex(ID id, Label label): _id(id), _label(std::move(label)) {}

void Vertex::visit(VertexVisitor& visitor) const { visitor.visit(*this); }

void Graph::visit(VertexVisitor& visitor) const { visitor.visit(*this); }
