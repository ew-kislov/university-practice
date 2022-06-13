#pragma once

#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

namespace uni_course_cpp {

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;
  using Depth = int;

  static constexpr Depth kInitialDepth = 1;

  struct Vertex {
    explicit Vertex(VertexId init_id) : id(init_id) {}

    const VertexId id = 0;
  };

  struct Edge {
    enum class Color { Grey, Green, Yellow, Red };

    Edge(EdgeId init_id,
         VertexId init_from_vertex_id,
         VertexId init_to_vertex_id,
         Color init_color)
        : id(init_id),
          from_vertex_id(init_from_vertex_id),
          to_vertex_id(init_to_vertex_id),
          color(init_color) {}

    const EdgeId id = 0;
    const VertexId from_vertex_id = 0;
    const VertexId to_vertex_id = 0;
    const Color color;
  };

  VertexId add_vertex();
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id);

  const std::unordered_map<EdgeId, Edge>& get_edges() const;
  const std::unordered_map<VertexId, Vertex>& get_vertices() const;

  const std::set<Graph::VertexId>& get_vertex_ids_by_depth(Depth depth) const;

  const std::vector<EdgeId>& get_edges_ids_for_vertex(VertexId vertex_id) const;

  Depth get_vertex_depth(Graph::VertexId id) const;

  Depth get_depth() const;

  bool is_connected(VertexId from_vertex_id, VertexId to_vertex_id) const;

  const std::set<EdgeId>& get_edge_ids_by_color(Edge::Color color) const;

 private:
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, Vertex> vertices_;

  std::unordered_map<Depth, std::set<EdgeId>> vertex_ids_by_depths_;

  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;

  std::unordered_map<VertexId, Depth> vertex_depths_;

  std::unordered_map<Edge::Color, std::set<EdgeId>> colored_edges_;

  EdgeId edge_id_counter_ = 0;
  VertexId vertex_id_counter_ = 0;

  VertexId get_new_vertex_id();
  EdgeId get_new_edge_id();
  bool has_vertex(VertexId id) const;

  void set_vertex_depth(VertexId vertex_id, Depth depth);
  void update_vertex_depth(VertexId vertex_id, Depth depth);

  Edge::Color calculate_edge_color(VertexId from_vertex_id,
                                   VertexId to_vertex_id) const;
};

}  // namespace uni_course_cpp
