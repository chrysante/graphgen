#include <filesystem>
#include <fstream>
#include <iostream>

#include <graphgen/graphgen.h>

using namespace graphgen;

int main(int argc, char const* argv[]) {
    auto G = std::make_unique<Graph>(0);

    G->kind(GraphKind::Directed)
        ->font("SF Mono")
        ->add(Vertex::make(1)->label("Label A")->font("Helvetica"))
        ->add(Vertex::make(2)->label("Label B")->shape(VertexShape::Circle))
        ->add(Vertex::make(3)->label("Label C")->shape(VertexShape::Ellipse))
        ->add(Edge{ 1, 2 })
        ->add(Edge{ 2, 3 })
        ->add(Edge{ 3, 1 });

    auto H = std::make_unique<Graph>(4);
    H->label("Subgraph")
        ->add(Vertex::make(5)->label("Sub A"))
        ->add(Vertex::make(6)->label("Sub B"))
        ->add({ 5, 6 });
    G->add(std::move(H));

    auto table =
        R"(<table border="0" cellborder="0" cellspacing="0">
        <tr><td align="left"><font face="SF Mono">
<font color="">%entry</font>:
        </font></td></tr>
        <tr><td align="left"><font face="SF Mono">
    <font color="">%cmp.res</font> <font color="">=</font> <font color="MediumVioletRed">scmp</font> <font color="MediumVioletRed">ls</font> <font color="CornflowerBlue">i64</font> <font color="">%0</font>, <font color="CornflowerBlue">i64</font> <font color="">%1</font>
        </font></td></tr>
        <tr><td align="left"><font face="SF Mono">
    <font color="MediumVioletRed">branch</font> <font color="CornflowerBlue">i1</font> <font color="">%cmp.res</font>, <font color="LightSlateGray">label</font> <font color="">%cond.then</font>, <font color="LightSlateGray">label</font> <font color="">%cond.else</font>
        </font></td></tr>
        </table>)";

    H = std::make_unique<Graph>(7);
    H->add(Vertex::make(8)->label("Sub A"))
        ->add(Vertex::make(9)->label(table, LabelKind::HTML))
        ->add({ 8, 9 });
    G->add(std::move(H));

    generate(*G);

    if (argc < 2) {
        std::cout << "Please specify dest path\n";
        return -1;
    }
    auto destpath = std::filesystem::path(argv[1]);
    std::fstream file(destpath, std::ios::out | std::ios::trunc);
    if (!file) {
        std::cout << "Failed to open file\n";
        std::exit(-1);
    }
    generate(*G, file);
    return 0;
}
