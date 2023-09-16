#include "Generate.h"

#include <array>
#include <iomanip>
#include <iostream>

#include "Graph.h"
#include "StreamManip.h"

using namespace graphgen;

static std::ostream& operator<<(std::ostream& str, ID id) {
    return str << "vertex_" << id.raw();
}

static std::ostream& operator<<(std::ostream& str, GraphKind kind) {
    switch (kind) {
    case GraphKind::Directed:
        return str << "digraph";
    case GraphKind::Undirected:
        return str << "graph";
    case GraphKind::Tree:
        assert(false);
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

struct Context: VertexVisitor {
    Graph const& graph;
    std::ostream& str;

    int currentIndent = 0;

    Context(Graph const& graph, std::ostream& str, int indent = 0):
        graph(graph), str(str), currentIndent(indent) {}

    void beginScope(ScopeKind kind, auto const&... args) {
        line(args..., " ", open(kind));
        ++currentIndent;
    }

    void endScope(ScopeKind kind, auto const&... args) {
        --currentIndent;
        if constexpr (sizeof...(args) > 0) {
            line(close(kind), " // ", args...);
        }
        else {
            line(close(kind));
        }
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
    beginScope(Brace, declare(graph));
    commonDecls(graph);
    for (auto* vertex: graph.vertices()) {
        vertex->visit(*this);
    }
    for (auto& edge: graph.edges()) {
        generate(edge);
    }
    endScope(Brace, graph.kind());
}

void Context::visit(Vertex const& vertex) {
    beginScope(Bracket, vertex.id());
    commonDecls(vertex);
    endScope(Bracket);
}

void Context::commonDecls(Vertex const& vertex) {
    line("label = ", vertex.label());
    line("fontname = ", std::quoted(vertex.font()));
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
