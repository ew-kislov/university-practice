#include "graph.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace uni_course_cpp {

Graph::VertexId Graph::add_vertex() {
  const Graph::VertexId new_vertex_id = get_new_vertex_id();

  vertices_.emplace(new_vertex_id, Graph::Vertex(new_vertex_id));
  adjacency_list_.emplace(new_vertex_id, std::vector<EdgeId>());

  set_vertex_depth(new_vertex_id, kInitialDepth);

  return new_vertex_id;
}

void Graph::add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
  assert(has_vertex(from_vertex_id) && "Vertex from_vertex_id doesn't exist");
  assert(has_vertex(to_vertex_id) && "Vertex to_vertex_id doesn't exist");

  const Graph::EdgeId new_edge_id = get_new_edge_id();

  const Graph::Edge::Color edge_color =
      calculate_edge_color(from_vertex_id, to_vertex_id);

  edges_.emplace(new_edge_id, Graph::Edge(new_edge_id, from_vertex_id,
                                          to_vertex_id, edge_color));

  colored_edges_[edge_color].emplace(new_edge_id);

  if (from_vertex_id != to_vertex_id) {
    adjacency_list_[to_vertex_id].push_back(new_edge_id);
  }

  adjacency_list_[from_vertex_id].push_back(new_edge_id);

  const Depth from_vertex_depth = get_vertex_depth(from_vertex_id);

  if (edge_color == Edge::Color::Grey) {
    update_vertex_depth(to_vertex_id, from_vertex_depth + 1);
  }
}

const std::vector<Graph::EdgeId>& Graph::get_edges_ids_for_vertex(
    VertexId vertex_id) const {
  return adjacency_list_.at(vertex_id);
}

const std::set<Graph::VertexId>& Graph::get_vertex_ids_by_depth(
    Depth depth) const {
  if (vertex_ids_by_depths_.find(depth) == vertex_ids_by_depths_.end()) {
    static const std::set<VertexId> empty_set;

    return empty_set;
  }

  return vertex_ids_by_depths_.at(depth);
}

const std::unordered_map<Graph::EdgeId, Graph::Edge>& Graph::get_edges() const {
  return edges_;
}

const std::unordered_map<Graph::VertexId, Graph::Vertex>& Graph::get_vertices()
    const {
  return vertices_;
}

Graph::VertexId Graph::get_new_vertex_id() {
  return vertex_id_counter_++;
}

Graph::EdgeId Graph::get_new_edge_id() {
  return edge_id_counter_++;
}

bool Graph::has_vertex(VertexId id) const {
  return vertices_.find(id) != vertices_.end();
}

Graph::Depth Graph::get_vertex_depth(Graph::VertexId id) const {
  return vertex_depths_.at(id);
}

void Graph::set_vertex_depth(VertexId vertex_id, Depth depth) {
  vertex_ids_by_depths_[depth].insert(vertex_id);
  vertex_depths_[vertex_id] = depth;
}

void Graph::update_vertex_depth(VertexId vertex_id, Depth depth) {
  Depth old_depth = get_vertex_depth(vertex_id);

  vertex_ids_by_depths_[old_depth].erase(vertex_id);

  set_vertex_depth(vertex_id, depth);
}

Graph::Edge::Color Graph::calculate_edge_color(
    Graph::VertexId from_vertex_id,
    Graph::VertexId to_vertex_id) const {
  const auto from_vertex_depth = get_vertex_depth(from_vertex_id);
  const auto to_vertex_depth = get_vertex_depth(to_vertex_id);
  if (from_vertex_id == to_vertex_id) {
    return Edge::Color::Green;
  }
  if (adjacency_list_.at(to_vertex_id).size() == 0) {
    return Edge::Color::Grey;
  }
  if (to_vertex_depth - from_vertex_depth == 1 &&
      !is_connected(from_vertex_id, to_vertex_id)) {
    return Edge::Color::Yellow;
  }
  if (to_vertex_depth - from_vertex_depth == 2) {
    return Edge::Color::Red;
  }

  throw std::runtime_error("Failed to determine color");
}

bool Graph::is_connected(Graph::VertexId from_vertex_id,
                         Graph::VertexId to_vertex_id) const {
  for (const Graph::EdgeId& edge_id : adjacency_list_.at(from_vertex_id)) {
    if (edges_.at(edge_id).to_vertex_id == to_vertex_id) {
      return true;
    }
  }

  for (const Graph::EdgeId& edge_id : adjacency_list_.at(to_vertex_id)) {
    if (edges_.at(edge_id).to_vertex_id == from_vertex_id) {
      return true;
    }
  }

  return false;
}

Graph::Depth Graph::get_depth() const {
  return vertex_ids_by_depths_.size();
}

const std::set<Graph::EdgeId>& Graph::get_edge_ids_by_color(
    Edge::Color color) const {
  if (colored_edges_.find(color) == colored_edges_.end()) {
    static const std::set<VertexId> empty_set;
    return empty_set;
  }
  return colored_edges_.at(color);
}

}  // namespace uni_course_cpp
