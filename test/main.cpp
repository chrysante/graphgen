#include <fstream>
#include <iostream>

#include "graphgen/generate.h"
#include "graphgen/graph.h"

using namespace graphgen;

int main(int argc, char const* argv[]) {

    auto G = std::make_unique<Graph>(0, GraphKind::Directed);

    G->add(Vertex::make(1)->label("Label A")->font("Helvetice"));
    G->add(Vertex::make(2)->label("Label B")->shape(VertexShape::Circle));
    G->add(Vertex::make(3)->label("Label C")->shape(VertexShape::Ellipse));
    G->add(Edge{ 1, 2 });
    G->add(Edge{ 2, 3 });
    G->add(Edge{ 3, 1 });

    auto H = std::make_unique<Graph>(4, GraphKind::Directed);
    H->label("Subgraph");
    H->add(Vertex::make(5)->label("Sub A"));
    H->add(Vertex::make(6)->label("Sub B"));
    H->add({ 5, 6 });
    G->add(std::move(H));

    H = std::make_unique<Graph>(7, GraphKind::Directed);
    H->add(Vertex::make(8)->label("Sub A"));
    H->add(Vertex::make(9)->label("<font color=\"MediumVioletRed\">scmp</font>",
                                  LabelKind::HTML));
    H->add({ 8, 9 });
    G->add(std::move(H));

    generate(*G);

    std::fstream file("/Users/indigo/dev/graphgen/tmp/test.gv",
                      std::ios::out | std::ios::trunc);
    if (!file) {
        std::cout << "Failed to open file\n";
        std::exit(-1);
    }

    generate(*G, file);

    return 0;
}
