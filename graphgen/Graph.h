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

class Vertex;
class Graph;
class VertexVisitor;

/// Vertex identifier. This is used to identify vertices when declaring edges
class ID {
public:
    ID(void const volatile* ptr): _id(reinterpret_cast<uintptr_t>(ptr)) {}

    ID(int id): _id(id) {}

    ID(size_t id): _id(id) {}

    /// \Returns the raw value of the ID
    uintptr_t raw() const { return _id; }

private:
    uintptr_t _id;
};

/// Different kinds of labels
/// - `PlainText` is the default. When this option is used, double quotes `"`
///   will be added around the label in the generated graphviz code
/// - `HTML` When this option is used, `<` and `>` will be inserted around the
///   label in the generated code
enum class LabelKind { PlainText, HTML };

/// Represents a label of a vertex.
class Label {
public:
    /// Constructs a label from \p text with label kind \p kind
    Label(std::string text, LabelKind kind = LabelKind::PlainText);

    /// \overload for `std::string_view`
    Label(std::string_view text, LabelKind kind = LabelKind::PlainText):
        Label(std::string(text), kind) {}

    /// \overload for `char const*`
    Label(char const* text, LabelKind kind = LabelKind::PlainText):
        Label(std::string(text), kind) {}

    ///
    Label(std::function<void(std::ostream&)> generator,
          LabelKind kind = LabelKind::PlainText);

    /// \Returns The kind of the label
    LabelKind kind() const { return _kind; }

    /// Writes the label to \p ostream
    friend std::ostream& operator<<(std::ostream& ostream, Label const& label) {
        label.emit(ostream);
        return ostream;
    }

private:
    void emit(std::ostream& str) const;

    std::function<void(std::ostream&)> generator;
    LabelKind _kind;
};

/// Different shapes of vertices
enum class VertexShape { Box, Ellipse, Oval, Circle, Point };

/// Represents a vertex in the graph
class Vertex {
public:
    Vertex(ID id, Label label);

    virtual ~Vertex() = default;

    /// \Returns the ID of the vertex
    ID id() const { return _id; }

    /// \Returns the label of the vertex
    Label const& label() const { return _label; }

    /// \Returns the shape of the vertex
    VertexShape shape() const { return _shape; }

    /// Set the shape of this vertex
    Vertex* setShape(VertexShape shape) {
        _shape = shape;
        return this;
    }

    /// \Returns the font used for the vertex if overriden
    std::optional<std::string> font() const { return _font; }

    /// Override the font used for this vertex
    Vertex* setFont(std::string fontname) {
        _font = std::move(fontname);
        return this;
    }

    /// \Returns the parent vertex in the graph
    Vertex* parent() { return _parent; }

    /// \overload for const
    Vertex const* parent() const { return _parent; }

    /// Visitor pattern. This is actually an implementation detail
    virtual void visit(VertexVisitor& visitor) const;

private:
    friend class Graph;
    Vertex* _parent = nullptr;
    ID _id;
    Label _label;
    VertexShape _shape{};
    std::optional<std::string> _font;
};

/// Represents an edge between the vertices with IDs \p from and \p to
struct Edge {
    ID from;
    ID to;
};

/// Different kinds of graphs
/// `Tree` is not supported yet
enum class GraphKind { Directed, Undirected, Tree };

///
class Graph: public Vertex {
public:
    explicit Graph(ID id, Label label, GraphKind kind):
        Vertex(id, std::move(label)), _kind(kind) {}

    /// Adds \p vertex to the graph
    /// \Returns the ID of the added vertex
    /// This function expects the pointer to be allocated with `new` and will
    /// take ownership. This exists for clean callsites:
    /// ```
    ///  graph.add(new Vertex(...));
    /// ```
    ID add(Vertex* vertex) {
        _vertices.push_back(std::unique_ptr<Vertex>(vertex));
        vertex->_parent = this;
        return vertex->id();
    }

    /// \overload for `unique_ptr<Vertex>`
    ID add(std::unique_ptr<Vertex> vertex) { return add(vertex.release()); }

    /// Adds the edge \p edge to the graph
    void add(Edge edge) { _edges.push_back(edge); }

    /// \Returns the kind of the graph
    GraphKind kind() const { return _kind; }

    /// \Returns a view over the vertices of this graph
    auto vertices() const {
        return _vertices |
               std::views::transform([](auto& ptr) { return ptr.get(); });
    }

    /// \Returns a view over the edges of this graph
    std::span<Edge const> edges() const { return _edges; }

    /// Visitor pattern
    void visit(VertexVisitor& visitor) const override;

private:
    GraphKind _kind;
    std::vector<std::unique_ptr<Vertex>> _vertices;
    std::vector<Edge> _edges;
    bool _isSubgraph = false;
};

} // namespace graphgen

#endif // GRAPHGEN_GRAPH_H_
