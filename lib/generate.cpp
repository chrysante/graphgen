#include "graphgen/generate.h"

#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>

#include "config.h"
#include "graph.h"
#include "util.h"
#include "vertexvisitor.h"

using namespace graphgen;

static std::ostream& operator<<(std::ostream& str, ID id) {
    return str << "vertex_" << id.raw();
}

static std::ostream& operator<<(std::ostream& str, GraphKind kind) {
    using enum GraphKind;
    switch (kind) {
    case Directed:
        return str << "digraph";
    case Undirected:
        return str << "graph";
    case Tree:
        assert(false);
    }
}

static std::ostream& operator<<(std::ostream& str, RankDir dir) {
    using enum RankDir;
    switch (dir) {
    case TopBottom:
        return str << "TB";
    case LeftRight:
        return str << "LR";
    case BottomTop:
        return str << "BT";
    case RightLeft:
        return str << "RL";
    }
}

static std::ostream& operator<<(std::ostream& str, VertexShape shape) {
    using enum VertexShape;
    switch (shape) {
    case Box:
        return str << std::quoted("box");
    case Ellipse:
        return str << std::quoted("ellipse");
    case Oval:
        return str << std::quoted("oval");
    case Circle:
        return str << std::quoted("circle");
    case Point:
        return str << std::quoted("point");
    }
}

StreamManip declare = [](std::ostream& str, Graph const& graph) {
    if (graph.parent()) {
        str << "subgraph cluster_" << graph.id();
    }
    else {
        str << graph.kind();
    }
};

namespace {

enum ScopeKind { Brace, Bracket };

static char const* open(ScopeKind kind) {
    return std::array{ "{", "[" }[static_cast<size_t>(kind)];
}

static char const* close(ScopeKind kind) {
    return std::array{ "}", "]" }[static_cast<size_t>(kind)];
}

struct Scope {
    ScopeKind kind;
    std::string name;
};

struct Context: VertexVisitor {
    Graph const& graph;
    std::ostream& str;

    int currentIndent = 0;
    std::stack<Scope> openScopes;
    std::stack<std::string> fontStack;

    Context(Graph const& graph, std::ostream& str, int indent = 0):
        graph(graph), str(str), currentIndent(indent) {}

    [[nodiscard]] auto beginScope(Vertex const& vertex,
                                  ScopeKind kind,
                                  auto const&... args) {
        beginScopeImpl(kind, args...);
        if (vertex.font()) {
            fontStack.push(*vertex.font());
        }
        return ScopeGuard([this, &vertex] {
            endScopeImpl();
            if (vertex.font()) {
                fontStack.pop();
            }
        });
    }

    void beginScopeImpl(ScopeKind kind, auto const&... args) {
        std::stringstream sstr;
        ((sstr << args), ...);
        openScopes.push({ kind, std::move(sstr).str() });
        line(openScopes.top().name, " ", open(kind));
        ++currentIndent;
    }

    void endScopeImpl() {
        --currentIndent;
        Scope scope = openScopes.top();
        openScopes.pop();
        line(close(scope.kind), " // ", scope.name);
    }

    void line(auto const&... args) {
        indent();
        ((str << args), ...);
        str << "\n";
    }

    void indent() {
        for (int i = 0; i < currentIndent; ++i) {
            str << "    ";
        }
    }

    std::string getFont(Vertex const& vertex) const {
        if (vertex.font()) {
            return *vertex.font();
        }
        if (!fontStack.empty()) {
            return fontStack.top();
        }
        return defaultFont();
    }

    void run() { graph.visit(*this); }

    void visit(Graph const& graph) override;

    void visit(Vertex const& vertex) override;

    void commonDecls(Vertex const& vertex);

    void generate(Edge edge);
};

} // namespace

void graphgen::generate(Graph const& graph, std::ostream& ostream) {
    Context(graph, ostream).run();
}

void graphgen::generate(Graph const& graph) { generate(graph, std::cout); }

void Context::visit(Graph const& graph) {
    auto scope = beginScope(graph, Brace, declare(graph));
    commonDecls(graph);
    line("rankdir = ", graph.rankdir());
    for (auto* vertex: graph.vertices()) {
        vertex->visit(*this);
    }
    for (auto& edge: graph.edges()) {
        generate(edge);
    }
}

void Context::visit(Vertex const& vertex) {
    auto scope = beginScope(vertex, Bracket, vertex.id());
    commonDecls(vertex);
}

void Context::commonDecls(Vertex const& vertex) {
    line("label = ", vertex.label());
    line("fontname = ", std::quoted(getFont(vertex)));
    line("shape = ", vertex.shape());
}

static std::string_view toString(Color color) {
    using enum Color;
    switch (color) {
    case Black:
        return "black";
    case White:
        return "white";
    case Red:
        return "red";
    case Green:
        return "green";
    case Yellow:
        return "yellow";
    case Blue:
        return "blue";
    case Magenta:
        return "magenta";
    case Purple:
        return "purple";
    }
}

static std::string_view toString(Style style) {
    using enum Style;
    switch (style) {
    case Dashed:
        return "dashed";
    case Dotted:
        return "dotted";
    case Solid:
        return "solid";
    case Invisible:
        return "invis";
    case Bold:
        return "bold";
    }
}

static StreamManip makeEdge = [](std::ostream& str, Edge edge, GraphKind kind) {
    str << edge.from;
    switch (kind) {
    case GraphKind::Directed:
        str << " -> ";
        break;
    case GraphKind::Undirected:
        str << " -- ";
        break;
    case GraphKind::Tree:
        assert(false);
        break;
    }
    str << edge.to;
    if (edge.color) {
        str << " [color=\"" << toString(*edge.color) << "\"]";
    }
    if (edge.style) {
        str << " [style=\"" << toString(*edge.style) << "\"]";
    }
};

void Context::generate(Edge edge) { line(makeEdge(edge, graph.kind())); }
