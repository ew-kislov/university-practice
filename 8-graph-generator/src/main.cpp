#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "config.hpp"

#include "graph.hpp"
#include "graph_generation_controller.hpp"
#include "graph_generator.hpp"
#include "graph_json_printing.hpp"
#include "graph_printing.hpp"
#include "logger.hpp"

int handle_depth_input() {
  int depth;

  std::cout << "Enter depth (a positive integer):" << std::endl;

  while (true) {
    std::cin >> depth;

    if (depth < 0) {
      std::cout << "Depth should be positive integer!" << std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
      return depth;
    }
  }
}

int handle_new_vertices_count_input() {
  int new_vertices_count;

  std::cout << "Enter new vertices count (a positive integer):" << std::endl;

  while (true) {
    std::cin >> new_vertices_count;

    if (new_vertices_count < 0) {
      std::cout << "New vertices count should be a positive integer!"
                << std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
      return new_vertices_count;
    }
  }
}

int handle_graphs_count_input() {
  int graphs_count;

  std::cout << "Enter graphs count (a positive integer):" << std::endl;

  while (true) {
    std::cin >> graphs_count;

    if (graphs_count < 0) {
      std::cout << "Graphs count should be a positive integer!" << std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
      return graphs_count;
    }
  }
}

int handle_threads_count_input() {
  int threads_count;

  std::cout << "Enter threads count (a positive integer):" << std::endl;

  while (true) {
    std::cin >> threads_count;

    if (threads_count < 0) {
      std::cout << "Threads count should be a positive integer!" << std::endl;
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
      return threads_count;
    }
  }
}

void write_to_file(const std::string& file_content,
                   const std::string& file_name) {
  std::ofstream file(file_name);
  file << file_content;
  file.close();
}

void prepare_temp_directory() {
  std::filesystem::remove_all(uni_course_cpp::config::kTempDirectoryPath);
  std::filesystem::create_directory(uni_course_cpp::config::kTempDirectoryPath);
}

std::string generation_started_string(int graph_count) {
  return "Graph " + std::to_string(graph_count) + ", Generation started";
}

std::string generation_finished_string(int graph_count,
                                       const std::string& graph_info) {
  return "Graph " + std::to_string(graph_count) + ", Generation finished " +
         graph_info + "\n";
}

std::vector<uni_course_cpp::Graph> generate_graphs(
    uni_course_cpp::GraphGenerator::Params&& params,
    int graphs_count,
    int threads_count) {
  auto generation_controller = uni_course_cpp::GraphGenerationController(
      threads_count, graphs_count, std::move(params));

  auto& logger = uni_course_cpp::Logger::get_instance();

  auto graphs = std::vector<uni_course_cpp::Graph>();
  graphs.reserve(graphs_count);

  generation_controller.generate(
      [&logger](int index) { logger.log(generation_started_string(index)); },
      [&logger, &graphs](int index, uni_course_cpp::Graph&& graph) {
        const auto graph_description =
            uni_course_cpp::printing::graph_to_string(graph);
        logger.log(generation_finished_string(index, graph_description));
        const auto graph_json =
            uni_course_cpp::printing::json::graph_to_json(graph);
        write_to_file(graph_json, "graph_" + std::to_string(index) + ".json");

        graphs.push_back(std::move(graph));
      });

  return graphs;
}

int main() {
  const int depth = handle_depth_input();
  const int new_vertices_count = handle_new_vertices_count_input();
  const int graphs_count = handle_graphs_count_input();
  const int threads_count = handle_threads_count_input();
  prepare_temp_directory();

  auto params =
      uni_course_cpp::GraphGenerator::Params(depth, new_vertices_count);
  const auto graphs =
      generate_graphs(std::move(params), graphs_count, threads_count);

  return 0;
}
