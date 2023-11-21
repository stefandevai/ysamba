#pragma once

#include <entt/entity/fwd.hpp>
#include <random>

namespace dl
{
class World;

class SocietySystem
{
 public:
  SocietySystem(World& world);

  void update(entt::registry& registry, const double delta);

 private:
  World& m_world;
  std::random_device m_rd;
  std::mt19937 m_rng{m_rd()};
  std::uniform_real_distribution<float> m_distribution{0.f, 1.f};
};
}  // namespace dl
