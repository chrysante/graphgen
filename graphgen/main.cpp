#include <fstream>
#include <iostream>

#include "Graph.h"
#include "Generate.h"

using namespace graphgen;

int main(int argc, const char * argv[]) {
    
    auto G = Graph(GraphKind::Undirected);
    
    G.addNode({ 0, R"("Label A")" });
    G.addNode({ 1, R"("Label B")" });
    G.addNode({ 2, R"("Label C")" });
    
    G.addEdge(0, 1);
    G.addEdge(1, 2);
    G.addEdge(2, 0);
    
    generate(G);
    
    std::fstream file("/Users/indigo/dev/graphgen/test.gv", std::ios::out | std::ios::trunc);
    if (!file) {
        std::cout << "Failed to open file\n";
        std::exit(-1);
    }
    
    generate(G, file);
    
    return 0;
}
