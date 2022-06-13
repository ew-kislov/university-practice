#include "graph_json_printing.hpp"

#include "graph_printing.hpp"

namespace uni_course_cpp {
namespace printing {
namespace json {

std::string graph_to_json(const Graph& graph) {
  std::string edges_string;
  std::string vertices_string;

  const std::unordered_map<Graph::EdgeId, Graph::Edge>& edges =
      graph.get_edges();
  const std::unordered_map<Graph::VertexId, Graph::Vertex>& vertices =
      graph.get_vertices();

  for (auto edges_id_it = edges.cbegin(); edges_id_it != edges.cend();
       edges_id_it++) {
    if (edges_id_it != edges.cbegin()) {
      edges_string += ",\n";
    }

    edges_string += "\t\t" + edge_to_json(edges_id_it->second);
  }

  for (auto vertices_id_it = vertices.cbegin();
       vertices_id_it != vertices.cend(); vertices_id_it++) {
    if (vertices_id_it != vertices.cbegin()) {
      vertices_string += ",\n";
    }

    vertices_string += "\t\t" + vertex_to_json(vertices_id_it->second, graph);
  }

  return "{\n\t\"depth\": " + std::to_string(graph.get_depth()) + "," +
         "\n\t\"vertices\": [\n" + vertices_string +
         "\n\t],\n\t\"edges\": [\n" + edges_string + "\n\t]\n}\n";
}

std::string vertex_to_json(const Graph::Vertex& vertex, const Graph& graph) {
  std::string edges_ids_string;

  const std::vector<Graph::EdgeId>& edges_ids =
      graph.get_edges_ids_for_vertex(vertex.id);

  for (auto edge_id_it = edges_ids.cbegin(); edge_id_it != edges_ids.cend();
       edge_id_it++) {
    if (edge_id_it != edges_ids.cbegin()) {
      edges_ids_string += ", ";
    }

    edges_ids_string += std::to_string(*edge_id_it);
  }

  const auto depth = graph.get_vertex_depth(vertex.id);

  return "{ \"id\": " + std::to_string(vertex.id) + ", \"edge_ids\": [" +
         edges_ids_string + "], \"depth\": \"" + std::to_string(depth) + "\" }";
}

std::string edge_to_json(const Graph::Edge& edge) {
  return "{ \"id\": " + std::to_string(edge.id) + ", \"vertex_ids\": [" +
         std::to_string(edge.from_vertex_id) + ", " +
         std::to_string(edge.to_vertex_id) + "], \"color\": \"" +
         edge_color_to_string(edge.color) + "\" }";
}

}  // namespace json
}  // namespace printing
}  // namespace uni_course_cpp
