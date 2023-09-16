#ifndef GRAPHGEN_GENERATE_H_
#define GRAPHGEN_GENERATE_H_

#include <iosfwd>

namespace graphgen {

class Graph;

void generate(Graph const& graph, std::ostream& ostream);

void generate(Graph const& graph);

} // namespace graphgen

#endif // GRAPHGEN_GENERATE_H_
