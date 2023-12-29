#include "./thread_pool.hpp"

#include <spdlog/spdlog.h>

namespace dl
{
void ThreadPool::initialize()
{
  const size_t max_threads = std::thread::hardware_concurrency();

  for (size_t i = 0; i < max_threads; ++i)
  {
    m_threads.emplace_back(std::thread(&ThreadPool::m_thread_loop, this));
  }
}

void ThreadPool::queue_job(const std::function<void()>& job)
{
  {
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    m_jobs.push(job);
  }
  m_mutex_condition.notify_one();
}

void ThreadPool::finalize()
{
  {
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    m_should_finalize = true;
  }

  m_mutex_condition.notify_all();

  for (auto& thread : m_threads)
  {
    thread.join();
  }

  m_threads.clear();
}

bool ThreadPool::is_busy()
{
  bool busy = true;

  {
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    busy = !m_jobs.empty();
  }

  return busy;
}

void ThreadPool::m_thread_loop()
{
  while (true)
  {
    std::function<void()> job;

    {
      std::unique_lock<std::mutex> lock(m_queue_mutex);

      m_mutex_condition.wait(lock, [this] { return !m_jobs.empty() || m_should_finalize; });

      if (m_should_finalize)
      {
        return;
      }

      job = m_jobs.front();
      m_jobs.pop();
    }

    if (job)
    {
      job();
    }
  }
}
}  // namespace dl
