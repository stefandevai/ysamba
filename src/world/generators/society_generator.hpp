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
  void generate_members(const int seed, Society& society, entt::registry& registry);

 private:
  // Generate members for a primitive communist society
  void m_generate_primitive_communism_members(Society& society, entt::registry& registry);

  // Generate members for a slavery based society
  /* void m_generate_slavery_members(const Society& society, entt::registry& registry); */

  // Create a person entity for the game
  void m_create_person_entity(entt::registry& registry, const Society& society, const uint32_t person_id);
};
}  // namespace dl
