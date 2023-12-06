#pragma once

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
  std::string society_id;
  std::string name{};
  SocialClass social_class;
  Metier metiers;
  State state = State::Idle;
  double time_to_next_action = 0.0;
  std::priority_queue<Job> jobs{};
};
}  // namespace dl
