#include "graph_generator.hpp"

#include <atomic>
#include <functional>
#include <iterator>
#include <list>
#include <optional>
#include <random>
#include <thread>

namespace uni_course_cpp {

namespace {
static constexpr float kGreenProbability = 0.1;
static constexpr float kRedProbability = 1.0 / 3.0;
static constexpr Graph::Depth kYellowDepth = 1;
static constexpr Graph::Depth kRedDepth = 2;

static const int kMaxThreadsCount = std::thread::hardware_concurrency();

bool should_generate_random_edge(float probability) {
  std::random_device rd;
  std::mt19937 gen(rd());

  std::bernoulli_distribution distribution(probability);

  return distribution(gen);
}

Graph::VertexId get_random_vertex_id(
    const std::set<Graph::VertexId>& vertex_ids) {
  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<int> distribution(0, vertex_ids.size() - 1);

  auto random_iterator = vertex_ids.begin();
  std::advance(random_iterator, distribution(gen));

  return *random_iterator;
}

std::set<Graph::VertexId> get_unconnected_vertex_ids(const Graph& graph,
                                                     Graph::VertexId vertex_id,
                                                     std::mutex& graph_mutex) {
  const auto depth = graph.get_vertex_depth(vertex_id);
  const auto& next_vertex_ids = graph.get_vertex_ids_by_depth(depth + 1);

  std::set<Graph::VertexId> unconnected_vertex_ids;

  for (const auto next_vertex_id : next_vertex_ids) {
    const auto is_connected = [&graph, vertex_id, next_vertex_id,
                               &graph_mutex]() {
      const std::lock_guard lock(graph_mutex);
      return graph.is_connected(vertex_id, next_vertex_id);
    }();

    if (!is_connected) {
      unconnected_vertex_ids.insert(next_vertex_id);
    }
  }

  return unconnected_vertex_ids;
}

}  // namespace

Graph GraphGenerator::generate() const {
  auto graph = Graph();

  if (params_.depth() == 0) {
    return graph;
  }

  const auto root_vertex_id = graph.add_vertex();

  if (params_.new_vertices_count() == 0) {
    return graph;
  }

  std::mutex graph_mutex;

  generate_grey_edges(graph, root_vertex_id, graph_mutex);

  std::thread green_thread([this, &graph, &graph_mutex]() {
    generate_green_edges(graph, graph_mutex);
  });

  std::thread yellow_thread([this, &graph, &graph_mutex]() {
    generate_yellow_edges(graph, graph_mutex);
  });

  std::thread red_thread([this, &graph, &graph_mutex]() {
    generate_red_edges(graph, graph_mutex);
  });

  green_thread.join();
  yellow_thread.join();
  red_thread.join();

  return graph;
}

void GraphGenerator::generate_grey_edges(Graph& graph,
                                         Graph::VertexId root_vertex_id,
                                         std::mutex& graph_mutex) const {
  using JobCallback = std::function<void()>;
  auto jobs = std::list<JobCallback>();
  std::mutex jobs_mutex;

  const float probability_step = 1.0 / (params_.depth() - 1);

  const auto new_vertices_count = params_.new_vertices_count();
  std::atomic<bool> should_terminate = false;

  for (int i = 0; i < new_vertices_count; i++) {
    jobs.push_back([this, &graph_mutex, &graph, root_vertex_id]() {
      generate_grey_branch(graph, root_vertex_id, graph_mutex);
    });
  }

  const auto worker = [&should_terminate, &jobs_mutex, &jobs]() {
    while (true) {
      if (should_terminate) {
        return;
      }

      const auto job_optional = [&jobs_mutex,
                                 &jobs]() -> std::optional<JobCallback> {
        const std::lock_guard lock(jobs_mutex);
        if (jobs.size()) {
          auto job = jobs.front();
          jobs.pop_front();
          return job;
        }
        return std::nullopt;
      }();

      if (job_optional.has_value()) {
        const auto& job = job_optional.value();
        job();
      }
    }
  };

  const auto threads_count = std::min(kMaxThreadsCount, new_vertices_count);
  auto threads = std::vector<std::thread>();
  threads.reserve(threads_count);

  for (int i = 0; i < threads_count; i++) {
    threads.emplace_back(std::thread(worker));
  }

  while (jobs.size()) {
  }

  should_terminate = true;
  for (auto& thread : threads) {
    thread.join();
  }
}

void GraphGenerator::generate_grey_branch(Graph& graph,
                                          Graph::VertexId from_vertex_id,
                                          std::mutex& graph_mutex) const {
  const auto vertices_count = params_.new_vertices_count();
  const double probability_step = 1.0 / (params_.depth() - 1);

  const Graph::Depth from_vertex_depth = graph.get_vertex_depth(from_vertex_id);

  if (!should_generate_random_edge(
          1 - probability_step * (from_vertex_depth - Graph::kInitialDepth))) {
    return;
  }

  const auto new_vertex_id = [&graph, from_vertex_id, &graph_mutex]() {
    const std::lock_guard lock(graph_mutex);

    const auto new_vertex_id = graph.add_vertex();
    graph.add_edge(from_vertex_id, new_vertex_id);

    return new_vertex_id;
  }();

  for (int i = 0; i < vertices_count; i++) {
    generate_grey_branch(graph, new_vertex_id, graph_mutex);
  }
}

void GraphGenerator::generate_green_edges(Graph& graph,
                                          std::mutex& graph_mutex) const {
  const auto& vertices = graph.get_vertices();
  std::for_each(vertices.cbegin(), vertices.cend(),
                [&graph, &graph_mutex](const auto& it) {
                  if (should_generate_random_edge(kGreenProbability)) {
                    const std::lock_guard lock(graph_mutex);
                    graph.add_edge(it.first, it.first);
                  }
                });
}

void GraphGenerator::generate_yellow_edges(Graph& graph,
                                           std::mutex& graph_mutex) const {
  const auto depth = graph.get_depth();

  const float probability_step = 1.0 / (depth - 2);

  for (Graph::Depth current_depth = Graph::kInitialDepth; current_depth < depth;
       current_depth++) {
    const float probability = probability_step * (current_depth - 1);

    const auto& current_depth_vertex_ids =
        graph.get_vertex_ids_by_depth(current_depth);

    std::for_each(
        current_depth_vertex_ids.cbegin(), current_depth_vertex_ids.cend(),
        [this, &graph, &graph_mutex, probability](auto vertex_id) {
          if (should_generate_random_edge(probability)) {
            const auto& unconnected_vertex_ids =
                get_unconnected_vertex_ids(graph, vertex_id, graph_mutex);

            if (unconnected_vertex_ids.size()) {
              const std::lock_guard lock(graph_mutex);
              graph.add_edge(vertex_id,
                             get_random_vertex_id(unconnected_vertex_ids));
            }
          }
        });
  }
}

void GraphGenerator::generate_red_edges(Graph& graph,
                                        std::mutex& graph_mutex) const {
  const auto depth = graph.get_depth();

  for (Graph::Depth current_depth = Graph::kInitialDepth;
       current_depth < depth - 1; current_depth++) {
    const auto& current_level_vertices =
        graph.get_vertex_ids_by_depth(current_depth);

    std::for_each(
        current_level_vertices.cbegin(), current_level_vertices.cend(),
        [this, &graph, &graph_mutex, current_depth](auto vertex_id) {
          if (should_generate_random_edge(kRedProbability)) {
            const auto& next_depth_vertex_ids =
                graph.get_vertex_ids_by_depth(current_depth + 2);
            if (next_depth_vertex_ids.size()) {
              const std::lock_guard lock(graph_mutex);
              graph.add_edge(vertex_id,
                             get_random_vertex_id(next_depth_vertex_ids));
            }
          }
        });
  }
}

}  // namespace uni_course_cpp
