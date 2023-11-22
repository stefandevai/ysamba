#pragma once

#include <entt/entity/fwd.hpp>

#include "../society.hpp"

namespace dl
{
class SocietyGenerator
{
 public:
  SocietyGenerator();

  // Generate society
  Society generate(const int seed);

  // Generate society members
  void generate_members(const int seed, const Society& society, entt::registry& registry) const;

 private:
  // Generate members for a primitive communist society
  void m_generate_primitive_communism_members(const Society& society, entt : registry& registry);

  // Generate members for a slavery based society
  void m_generate_slavery_members(const Society& society, entt : registry& registry);
};
}  // namespace dl
