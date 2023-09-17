#ifndef GRAPHGEN_VERTEXVISITOR_H_
#define GRAPHGEN_VERTEXVISITOR_H_

namespace graphgen {

class Vertex;
class Graph;

/// Interface for the visitor pattern on the graph
class VertexVisitor {
public:
    virtual ~VertexVisitor() = default;

    virtual void visit(Vertex const& vertex) = 0;

    virtual void visit(Graph const& vertex) = 0;
};

} // namespace graphgen

#endif // GRAPHGEN_VERTEXVISITOR_H_
