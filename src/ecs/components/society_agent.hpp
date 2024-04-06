#pragma once

#include <entt/entity/registry.hpp>
#include <queue>
#include <string>

#include "core/maths/vector.hpp"
#include "world/society/job.hpp"

namespace dl
{
enum class SocialClass
{
  None,
  Slave,
  Plebeian,
  Ruler,
};

enum class Metier
{
  None,
  Farmer,
  Warrior,
  Priest,
  Bricklayer,
  Dressmaker,
  Cook,
  Cacique,
};

struct SocietyAgent
{
  enum class State
  {
    Idle,
    Walking,
    Harvesting,
  };

  uint32_t id;
  uint32_t society_id;
  std::string name{};
  SocialClass social_class;
  Metier metiers;
  State state = State::Idle;
  double time_to_next_action = 0.0;
  std::vector<Job> jobs{};

  void push_job(Job job)
  {
    jobs.push_back(std::move(job));
    std::push_heap(jobs.begin(), jobs.end());
  }

  void pop_job()
  {
    std::pop_heap(jobs.begin(), jobs.end());
    jobs.pop_back();
  }

  void clear_jobs(entt::registry& registry)
  {
    for (auto& job : jobs)
    {
      registry.destroy(job.entity);
    }
    jobs.clear();
  }
};

template <typename Archive>
void serialize(Archive& archive, SocietyAgent& agent)
{
  archive(agent.id,
          agent.society_id,
          agent.name,
          agent.social_class,
          agent.metiers,
          agent.state,
          agent.time_to_next_action,
          agent.jobs);
}
}  // namespace dl
