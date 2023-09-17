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

    Context(Graph const& graph, std::ostream& str, int indent = 0):
        graph(graph), str(str), currentIndent(indent) {}

    [[nodiscard]] auto beginScope(ScopeKind kind, auto const&... args) {
        beginScopeImpl(kind, args...);
        return ScopeGuard([this] { endScopeImpl(); });
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
    auto scope = beginScope(Brace, declare(graph));
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
    auto scope = beginScope(Bracket, vertex.id());
    commonDecls(vertex);
}

void Context::commonDecls(Vertex const& vertex) {
    line("label = ", vertex.label());
    if (auto font = vertex.font()) {
        line("fontname = ", std::quoted(*font));
    }
    line("shape = ", vertex.shape());
}

static StreamManip makeEdge = [](std::ostream& str, Edge edge, GraphKind kind) {
    str << edge.to;
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
    str << edge.from;
};

void Context::generate(Edge edge) { line(makeEdge(edge, graph.kind())); }