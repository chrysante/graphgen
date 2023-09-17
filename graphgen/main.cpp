#include <fstream>
#include <iostream>

#include "Generate.h"
#include "Graph.h"

using namespace graphgen;

int main(int argc, char const* argv[]) {

    auto G = std::make_unique<Graph>(0, Label("main"), GraphKind::Directed);

    G->add(new Vertex(1, Label("Label A")));
    G->add(new Vertex(2, Label("Label B")));
    G->add(new Vertex(3, Label("Label C")));
    G->add(Edge{ 1, 2 });
    G->add(Edge{ 2, 3 });
    G->add(Edge{ 3, 1 });

    auto H = std::make_unique<Graph>(4, Label("Subgraph"), GraphKind::Directed);
    H->add(new Vertex(5, Label("Sub A")));
    H->add(new Vertex(6, Label("Sub B")));
    H->add({ 5, 6 });
    G->add(std::move(H));

    H = std::make_unique<Graph>(7, Label("Subgraph"), GraphKind::Directed);
    H->add(new Vertex(8, Label("Sub A")));
    H->add(new Vertex(9,
                      Label("<font color=\"MediumVioletRed\">scmp</font>",
                            LabelKind::HTML)));
    H->add({ 8, 9 });
    G->add(std::move(H));

    generate(*G);

    std::fstream file("/Users/indigo/dev/graphgen/test.gv",
                      std::ios::out | std::ios::trunc);
    if (!file) {
        std::cout << "Failed to open file\n";
        std::exit(-1);
    }

    generate(*G, file);

    return 0;
}
