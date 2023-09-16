#include "Graph.h"

#include <ostream>

using namespace graphgen;

Node::Node(ID id, std::string label):
    _label([label = std::move(label)](std::ostream& str) {
        str << label;
    }),
    _id(id) {}

Node::Node(ID id, std::function<void(std::ostream&)> label):
    _label(std::move(label)), _id(id) {}

void Graph::addSubgraph(Graph subgraph) {
    subgraph._isSubgraph = true;
    _subgraphs.push_back(std::move(subgraph));
}
