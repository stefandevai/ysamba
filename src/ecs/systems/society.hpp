#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;

class SocietySystem
{
 public:
  SocietySystem();

  void update(entt::registry& registry, const double delta);
};
}  // namespace dl
