#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <vector>

namespace dl
{
class ThreadPool
{
 public:
  void initialize();
  void queue_job(const std::function<void()>& job);
  void finalize();
  bool is_busy();

 private:
  bool m_should_finalize = false;
  std::mutex m_queue_mutex;
  std::condition_variable m_mutex_condition;
  std::vector<std::thread> m_threads;
  std::queue<std::function<void()>> m_jobs;

  void m_thread_loop();
};
}  // namespace dl
