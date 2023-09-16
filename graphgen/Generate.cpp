#include "Generate.h"

#include <iostream>

#include "Graph.h"
#include "StreamManip.h"

using namespace graphgen;

static constexpr auto monoFont = "SF Mono";

static constexpr StreamManip tableBegin = [](std::ostream& str,
                                                  int border = 0,
                                                  int cellborder = 0,
                                                  int cellspacing = 0) {
    str << "<table border=\"" << border << "\" cellborder=\"" << cellborder
        << "\" cellspacing=\"" << cellspacing << "\">";
};

static constexpr StreamManip tableEnd = [](std::ostream& str) {
    str << "</table>";
};

static constexpr StreamManip fontBegin =
    [](std::ostream& str, std::string_view fontname) {
    str << "<font face=\"" << fontname << "\">";
};

static constexpr StreamManip fontEnd = [](std::ostream& str) {
    str << "</font>";
};

static constexpr StreamManip rowBegin = [](std::ostream& str) {
    str << "<tr><td align=\"left\">";
};

static constexpr StreamManip rowEnd = [](std::ostream& str) {
    str << "</td></tr>";
};

std::ostream& operator<<(std::ostream& ostream, ID id) {
    return ostream << "node_" << id.raw();
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

namespace {

struct Context {
    Graph const& graph;
    std::ostream& str;
    
    int currentIndent = 0;
    
    Context(Graph const& graph, std::ostream& str, int indent = 0): graph(graph), str(str), currentIndent(indent) {}
    
    void beginScope(auto const&... args) {
        line(args..., "{");
        ++currentIndent;
    }
    
    void endScope() {
        --currentIndent;
        line("}");
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
    
    void run() {
        generate(graph);
    }
    
    void generate(Graph const& graph);
    
    void generate(Node const& node);
    
    void generate(Edge edge);
    
};

} // namespace

void graphgen::generate(Graph const& graph, std::ostream& ostream) {
    Context(graph, ostream).run();
}

void graphgen::generate(Graph const& graph) {
    generate(graph, std::cout);
}

void Context::generate(Graph const& graph) {
    beginScope(graph.kind());
    for (auto& subgraph: graph.subgraphs()) {
        Context(subgraph, str, currentIndent).run();
    }
    for (auto& node: graph.nodes()) {
        generate(node);
    }
    for (auto& edge: graph.edges()) {
        generate(edge);
    }
    endScope();
}

void Context::generate(Node const& node) {
    line(node.id(), " [ label = ", StreamManip(node.label()) ," ]");
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

void Context::generate(Edge edge) {
    line(makeEdge(edge, graph.kind()));
}
