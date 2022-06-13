#include "graph_printing.hpp"

#include <array>

namespace uni_course_cpp {
namespace printing {

namespace {

const std::unordered_map<Graph::Edge::Color, std::string>
    kEdgeColorStringMapping{{Graph::Edge::Color::Green, "green"},
                            {Graph::Edge::Color::Grey, "grey"},
                            {Graph::Edge::Color::Red, "red"},
                            {Graph::Edge::Color::Yellow, "yellow"}};

}

const std::string& edge_color_to_string(Graph::Edge::Color color) {
  return kEdgeColorStringMapping.at(color);
}

std::string graph_to_string(const Graph& graph) {
  return "{\n\tdepth: " + std::to_string(graph.get_depth()) +
         ",\n\tvertices: " + vertices_to_string(graph) + ",\n" +
         "\tedges: " + edges_to_string(graph) + ",\n" + "}\n";
}

std::string vertices_to_string(const Graph& graph) {
  std::string vertices_string;

  vertices_string +=
      "{ amount: " + std::to_string(graph.get_vertices().size()) +
      ", distribution: [";

  const auto depth = graph.get_depth();

  for (Graph::Depth current_depth = Graph::kInitialDepth;
       current_depth <= depth; current_depth++) {
    if (current_depth != Graph::kInitialDepth) {
      vertices_string += ", ";
    }
    vertices_string +=
        std::to_string(graph.get_vertex_ids_by_depth(current_depth).size());
  }

  vertices_string += "] }";

  return vertices_string;
}

std::string edges_to_string(const Graph& graph) {
  std::string edges_string;

  edges_string += "{ amount: " + std::to_string(graph.get_edges().size()) +
                  ", distribution: [";

  const std::array<Graph::Edge::Color, 4> edge_colors = {
      Graph::Edge::Color::Grey, Graph::Edge::Color::Green,
      Graph::Edge::Color::Yellow, Graph::Edge::Color::Red};

  for (auto color = edge_colors.cbegin(); color != edge_colors.cend();
       color++) {
    if (color != edge_colors.cbegin()) {
      edges_string += ", ";
    }
    edges_string += edge_color_to_string(*color) + ": " +
                    std::to_string(graph.get_edge_ids_by_color(*color).size());
  }

  edges_string += "] }";

  return edges_string;
}

}  // namespace printing
}  // namespace uni_course_cpp
