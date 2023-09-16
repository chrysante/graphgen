#ifndef GRAPHGEN_GRAPH_H_
#define GRAPHGEN_GRAPH_H_

#include <functional>
#include <iosfwd>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <vector>

namespace graphgen {

std::string defaultFont();

void setDefaultFont(std::string fontname);

class Vertex;
class Graph;

class VertexVisitor {
public:
    virtual ~VertexVisitor() = default;

    virtual void visit(Vertex const& vertex) = 0;

    virtual void visit(Graph const& vertex) = 0;
};

class ID {
public:
    ID(void const volatile* ptr): _id(reinterpret_cast<uintptr_t>(ptr)) {}

    ID(int id): _id(id) {}

    ID(size_t id): _id(id) {}

    uintptr_t raw() const { return _id; }

private:
    uintptr_t _id;
};

enum class GraphKind { Directed, Undirected, Tree };

enum class LabelKind { PlainText, HTML };

class Label {
public:
    Label(std::string label, LabelKind kind = LabelKind::PlainText);

    Label(std::function<void(std::ostream&)> generator,
          LabelKind kind = LabelKind::PlainText);

    LabelKind kind() const { return _kind; }

    friend std::ostream& operator<<(std::ostream& ostream, Label const& label) {
        label.emit(ostream);
        return ostream;
    }

private:
    void emit(std::ostream& str) const;

    std::function<void(std::ostream&)> generator;
    LabelKind _kind;
};

class Vertex {
public:
    Vertex(ID id, Label label);

    virtual ~Vertex() = default;

    ID id() const { return _id; }

    Label const& label() const { return _label; }

    Vertex* parent() { return _parent; }

    Vertex const* parent() const { return _parent; }

    virtual void visit(VertexVisitor& visitor) const { visitor.visit(*this); }

    std::string const& font() const { return _font; }

    void setFont(std::string fontname) { _font = std::move(fontname); }

private:
    friend class Graph;
    Vertex* _parent = nullptr;
    ID _id;
    Label _label;
    std::string _font;
};

struct Edge {
    ID from;
    ID to;
};

class Graph: public Vertex {
public:
    explicit Graph(ID id, Label label, GraphKind kind):
        Vertex(id, std::move(label)), _kind(kind) {}

    ID addVertex(Vertex* vertex) {
        _vertices.push_back(std::unique_ptr<Vertex>(vertex));
        vertex->_parent = this;
        return vertex->id();
    }

    ID addVertex(std::unique_ptr<Vertex> vertex) {
        return addVertex(vertex.release());
    }

    void addEdge(Edge edge) { _edges.push_back(edge); }

    void addEdge(ID from, ID to) { addEdge({ from, to }); }

    GraphKind kind() const { return _kind; }

    auto vertices() const {
        return _vertices |
               std::views::transform([](auto& ptr) { return ptr.get(); });
    }

    std::span<Edge const> edges() const { return _edges; }

    void visit(VertexVisitor& visitor) const override { visitor.visit(*this); }

private:
    GraphKind _kind;
    std::vector<std::unique_ptr<Vertex>> _vertices;
    std::vector<Edge> _edges;
    bool _isSubgraph = false;
};

} // namespace graphgen

#endif // GRAPHGEN_GRAPH_H_
