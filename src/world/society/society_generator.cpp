#include "./society_generator.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include "./name_generator.hpp"
#include "constants.hpp"
#include "core/maths/random.hpp"
#include "ecs/components/selectable.hpp"
#include "world/utils.hpp"
#include "world/world.hpp"

namespace dl
{
auto name_generator = NameGenerator();

SocietyBlueprint SocietyGenerator::generate_blueprint()
{
  using namespace entt::literals;

  SocietyBlueprint society{};

  society.id = "otomi"_hs;
  society.name = "Otomi";
  society.age = random::get_integer(148, 1848);
  society.predominance = static_cast<Predominance>(random::get_integer(0, 2));
  society.type = static_cast<SocietyType>(random::get_integer(0, 2));
  society.mode_of_production = ModeOfProduction::PrimitiveCommunism;

  return society;
}

std::vector<SocietyGenerator::MemberComponents> SocietyGenerator::generate_members(SocietyBlueprint& society)
{
  name_generator.load("guarani");
  std::vector<MemberComponents> members;
  const auto first_generation_members = 1;

  for (auto i = 0; i < first_generation_members; ++i)
  {
    const auto father_id = society.add_first_member(Sex::Male);
    MemberParameters father_parameters{};
    father_parameters.member_id = father_id;
    father_parameters.character_id = 0;
    father_parameters.movement_cost = 100;
    father_parameters.work_cost = 150;
    father_parameters.name = name_generator.generate();
    members.push_back(m_get_member_components(society, father_parameters));

    const auto mother_id = society.add_spouse(father_id);
    MemberParameters mother_parameters{};
    mother_parameters.member_id = mother_id;
    mother_parameters.character_id = 1;
    mother_parameters.movement_cost = 100;
    mother_parameters.work_cost = 150;
    mother_parameters.name = name_generator.generate();
    spdlog::info("Mothers's name: {}", mother_parameters.name);
    members.push_back(m_get_member_components(society, mother_parameters));

    const auto number_of_sons = 3;

    for (auto j = 0; j < number_of_sons; ++j)
    {
      const auto son_id = society.add_son(father_id);
      MemberParameters son_parameters{};
      son_parameters.member_id = son_id;
      son_parameters.character_id = 4;
      son_parameters.movement_cost = 120;
      son_parameters.work_cost = 200;
      son_parameters.name = name_generator.generate();
      spdlog::info("Sons's name: {}", son_parameters.name);
      members.push_back(m_get_member_components(society, son_parameters));
    }

    const auto number_of_daughters = 2;

    for (auto j = 0; j < number_of_daughters; ++j)
    {
      const auto daughter_id = society.add_daughter(father_id);
      MemberParameters daughter_parameters{};
      daughter_parameters.member_id = daughter_id;
      daughter_parameters.character_id = 5;
      daughter_parameters.movement_cost = 120;
      daughter_parameters.work_cost = 200;
      daughter_parameters.name = name_generator.generate();
      spdlog::info("Daughters's name: {}", daughter_parameters.name);
      members.push_back(m_get_member_components(society, daughter_parameters));
    }
  }

  return members;
}

void SocietyGenerator::place_members(std::vector<MemberComponents>& components,
                                     const World& world,
                                     entt::registry& registry,
                                     const Vector2i& map_position)
{
  using namespace entt::literals;

  const auto world_position = utils::map_to_world(map_position);

  for (auto& member : components)
  {
    auto entity = registry.create();
    registry.emplace<SocietyAgent>(entity, member.agent);
    registry.emplace<Biology>(entity, member.biology);
    registry.emplace<entt::tag<"collidable"_hs>>(entity, member.biology);

    const auto position = m_get_member_position(world, world_position);
    registry.emplace<Position>(entity, position);

    member.sprite.layer_z = renderer::layer_z_offset_characters;

    registry.emplace<Sprite>(entity, member.sprite);
    registry.emplace<CarriedItems>(entity, member.carried_items);
    registry.emplace<WearedItems>(entity, member.weared_items);
    registry.emplace<WieldedItems>(entity, member.wielded_items);
    registry.emplace<Selectable>(entity);
  }
}

Position SocietyGenerator::m_get_member_position(const World& world, const Vector2i& world_position)
{
  auto position = Position{0., 0., 0.};
  const uint32_t max_tries = 50;

  for (uint32_t tries = 0; tries < max_tries; ++tries)
  {
    const auto x = world_position.x + random::get_integer(32, 52);
    const auto y = world_position.y + random::get_integer(32, 52);

    const auto elevation = world.get_elevation(x, y);

    if (world.is_walkable(x, y, elevation))
    {
      position.x = x;
      position.y = y;
      position.z = elevation + 1;

      return position;
    }
  }

  spdlog::warn("Could not find position to place society member");

  return position;
}

SocietyGenerator::MemberComponents SocietyGenerator::m_get_member_components(const SocietyBlueprint& society,
                                                                             const MemberParameters& parameters)
{
  using namespace entt::literals;

  const auto& member = society.get_member(parameters.member_id);
  const auto agent = SocietyAgent{parameters.member_id, society.id, parameters.name, SocialClass::None, Metier::None};
  const auto carried_items = CarriedItems{};
  const auto weared_items = WearedItems{};
  const auto wielded_items = WieldedItems{};

  auto biology = Biology{member->sex, parameters.movement_cost, parameters.work_cost};

  // Defined in body_parts.json
  biology.body_parts = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

  return SocietyGenerator::MemberComponents{agent,
                                            biology,
                                            Sprite{
                                                .resource_id = "spritesheet-characters"_hs,
                                                .id = parameters.character_id,
                                                .category = "character",
                                            },
                                            carried_items,
                                            weared_items,
                                            wielded_items};
}

}  // namespace dl
