#pragma once

#include <string>

#include "graph.hpp"

namespace uni_course_cpp {
namespace printing {
namespace json {

std::string graph_to_json(const Graph& graph);
std::string vertex_to_json(const Graph::Vertex& vertex, const Graph& graph);
std::string edge_to_json(const Graph::Edge& edge);

}  // namespace json
}  // namespace printing
}  // namespace uni_course_cpp
