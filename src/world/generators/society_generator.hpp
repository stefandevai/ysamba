#pragma once

#include <entt/entity/registry.hpp>

#include "../society.hpp"

namespace dl
{
class SocietyGenerator
{
 public:
  SocietyGenerator();
  Society generate(const int seed);
  void generate_members(const int seed, const Society& society, entt::registry& registry) const;
};
}  // namespace dl
