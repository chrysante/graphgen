#ifndef GRAPHGEN_GRAPH_H_
#define GRAPHGEN_GRAPH_H_

#include <string>
#include <vector>
#include <span>
#include <functional>
#include <iosfwd>

namespace graphgen {

class ID {
public:
    ID(void const volatile* ptr): _id(reinterpret_cast<uintptr_t>(ptr)) {}

    ID(int id): _id(id) {}
    
    ID(size_t id): _id(id) {}
    
    uintptr_t raw() const { return _id; }
    
private:
    uintptr_t _id;
};

enum class GraphKind {
    Directed, Undirected, Tree
};

class Graph;

class Node {
public:
    Node(ID id, std::string label);
    
    Node(ID id, std::function<void(std::ostream&)> label);
    
    ID id() const { return _id; }
    
    auto const& label() const { return _label; }
    
private:
    friend class Graph;
    std::function<void(std::ostream&)> _label;
    ID _id;
};

struct Edge {
    ID from;
    ID to;
};

class Graph {
public:
    explicit Graph(GraphKind kind): _kind(kind) {}
    
    void addSubgraph(Graph subgraph);

    ID addNode(Node node) {
        _nodes.push_back(node);
        return node.id();
    }
    
    void addEdge(Edge edge) {
        _edges.push_back(edge);
    }
    
    void addEdge(ID from, ID to) {
        addEdge({ from, to });
    }
    
    GraphKind kind() const { return _kind; }
    
    bool isSubgraph() const { return _isSubgraph; }
    
    std::span<Graph const> subgraphs() const { return _subgraphs; }
    
    std::span<Node const> nodes() const { return _nodes; }
    
    std::span<Edge const> edges() const { return _edges; }
    
private:
    GraphKind _kind;
    std::vector<Node> _nodes;
    std::vector<Edge> _edges;
    std::vector<Graph> _subgraphs;
    bool _isSubgraph = false;
};

} // namespace graphgen

#endif // GRAPHGEN_GRAPH_H_
