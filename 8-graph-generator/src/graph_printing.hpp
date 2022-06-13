#pragma once

#include <string>
#include <unordered_map>

#include "graph.hpp"

namespace uni_course_cpp {
namespace printing {

const std::string& edge_color_to_string(Graph::Edge::Color color);
std::string graph_to_string(const Graph& graph);
std::string vertices_to_string(const Graph& graph);
std::string edges_to_string(const Graph& graph);

}  // namespace printing
}  // namespace uni_course_cpp
