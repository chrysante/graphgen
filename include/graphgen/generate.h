#ifndef GRAPHGEN_GENERATE_H_
#define GRAPHGEN_GENERATE_H_

#include <iosfwd>

#include <graphgen/api.h>

namespace graphgen {

class Graph;

/// Generate graphviz code for the graph \p graph and write it to \p ostream
GRAPHGEN_API void generate(Graph const& graph, std::ostream& ostream);

/// \overload for writing the generated code to `std::cout`
GRAPHGEN_API void generate(Graph const& graph);

} // namespace graphgen

#endif // GRAPHGEN_GENERATE_H_
