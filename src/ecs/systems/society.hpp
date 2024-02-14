#pragma once

#include <entt/entity/fwd.hpp>

namespace dl
{
class World;

class SocietySystem
{
 public:
  SocietySystem(const World& world);

  void update(entt::registry& registry, const double delta);

 private:
  const World& m_world;
};
}  // namespace dl
