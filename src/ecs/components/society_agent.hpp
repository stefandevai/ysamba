#pragma once

#include <string>

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
};
}  // namespace dl
