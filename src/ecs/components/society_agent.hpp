#pragma once

#include <queue>
#include <string>

#include "core/maths/vector.hpp"
#include "world/society/job2.hpp"

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
  std::priority_queue<Job2> jobs{};
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
