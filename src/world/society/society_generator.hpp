#pragma once

#include <entt/entity/fwd.hpp>

#include "./society_blueprint.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"

namespace dl
{
class Camera;
class World;

class SocietyGenerator
{
 public:
  // Components for a member entity
  struct MemberComponents
  {
    SocietyAgent agent;
    Biology biology;
    Sprite sprite;
    CarriedItems carried_items;
    WearedItems weared_items;
    WieldedItems wielded_items;
  };

  // Generate society
  static SocietyBlueprint generate_blueprint();

  // Generate society members
  [[nodiscard]] static std::vector<MemberComponents> generate_members(SocietyBlueprint& society);

  // Create actual members entities and add them to the world
  static void place_members(std::vector<MemberComponents>& components,
                            const World& world,
                            const Camera& camera,
                            entt::registry& registry);

 private:
  SocietyGenerator() = delete;

  // Parameters for society member creation
  struct MemberParameters
  {
    uint32_t member_id;
    uint32_t character_id;
    uint32_t movement_cost;
    std::string name;
  };

  // Member components factory method
  [[nodiscard]] static MemberComponents m_get_member_components(const SocietyBlueprint& society,
                                                                const MemberParameters& parameters);

  // Search a viable position in the world to place a member
  [[nodiscard]] static Position m_get_member_position(const World& world, const Camera& camera);
};
}  // namespace dl
