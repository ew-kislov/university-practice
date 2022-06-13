#include <cassert>
#include <mutex>

#include "graph_generation_controller.hpp"

namespace uni_course_cpp {

namespace {

static const int kMaxThreadsCount = std::thread::hardware_concurrency();

};

GraphGenerationController::GraphGenerationController(
    int threads_count,
    int graphs_count,
    GraphGenerator::Params&& graph_generator_params)
    : threads_count_(threads_count),
      graphs_count_(graphs_count),
      graph_generator_(std::move(graph_generator_params)) {
  Worker::GetJobCallback get_job_callback =
      [&jobs_ = jobs_,
       &jobs_mutex_ = jobs_mutex_]() -> std::optional<JobCallback> {
    const std::lock_guard lock(jobs_mutex_);

    if (jobs_.size()) {
      auto job = jobs_.front();
      jobs_.pop_front();
      return job;
    }

    return std::nullopt;
  };

  const auto actual_threads_count = std::min(kMaxThreadsCount, threads_count_);
  for (int i = 0; i < actual_threads_count; i++) {
    workers_.emplace_back(get_job_callback);
  }
};

void GraphGenerationController::generate(
    const GenStartedCallback& gen_started_callback,
    const GenFinishedCallback& gen_finished_callback) {
  std::mutex callback_mutex;

  for (int i = 0; i < graphs_count_; i++) {
    jobs_.emplace_back([&graph_generator_ = graph_generator_, &callback_mutex,
                        &gen_started_callback, &gen_finished_callback, i]() {
      {
        const std::lock_guard lock(callback_mutex);
        gen_started_callback(i);
      }

      auto graph = graph_generator_.generate();

      {
        const std::lock_guard lock(callback_mutex);
        gen_finished_callback(i, std::move(graph));
      }
    });
  }

  for (auto& worker : workers_) {
    worker.start();
  }

  while (jobs_.size()) {
  }

  for (auto& worker : workers_) {
    worker.stop();
  }
}

void GraphGenerationController::Worker::start() {
  assert(state_ == State::Working &&
         "Worker can't be in working state before start");

  state_ = State::Working;

  thread_ =
      std::thread([&state_ = state_, &get_job_callback_ = get_job_callback_]() {
        while (true) {
          if (state_ == State::ShouldTerminate) {
            state_ = State::Idle;
            return;
          }

          const auto job_optional = get_job_callback_();

          if (job_optional.has_value()) {
            const auto& job = job_optional.value();
            job();
          }
        }
      });
}

void GraphGenerationController::Worker::stop() {
  assert(state_ == State::Working &&
         "Worker can't be in working state when stopping");

  thread_.join();
}

GraphGenerationController::Worker::~Worker() {
  stop();
}
};  // namespace uni_course_cpp
