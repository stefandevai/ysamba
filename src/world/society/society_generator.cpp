#include "./society_generator.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include "./name_generator.hpp"
#include "constants.hpp"
#include "core/random.hpp"
#include "ecs/components/selectable.hpp"
#include "graphics/camera.hpp"
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
    auto father_parameters = MemberParameters();
    father_parameters.member_id = father_id;
    father_parameters.character_id = 0;
    father_parameters.speed = 100;
    father_parameters.name = name_generator.generate();
    members.push_back(m_get_member_components(society, father_parameters));

    const auto mother_id = society.add_spouse(father_id);
    auto mother_parameters = MemberParameters();
    mother_parameters.member_id = mother_id;
    mother_parameters.character_id = 1;
    mother_parameters.speed = 100;
    mother_parameters.name = name_generator.generate();
    spdlog::info("Mothers's name: {}", mother_parameters.name);
    members.push_back(m_get_member_components(society, mother_parameters));

    const auto number_of_sons = 3;

    for (auto j = 0; j < number_of_sons; ++j)
    {
      const auto son_id = society.add_son(father_id);
      auto son_parameters = MemberParameters();
      son_parameters.member_id = son_id;
      son_parameters.character_id = 4;
      // son_parameters.speed = 80;
      son_parameters.speed = 100;
      son_parameters.name = name_generator.generate();
      spdlog::info("Sons's name: {}", son_parameters.name);
      members.push_back(m_get_member_components(society, son_parameters));
    }

    const auto number_of_daughters = 2;

    for (auto j = 0; j < number_of_daughters; ++j)
    {
      const auto daughter_id = society.add_daughter(father_id);
      auto daughter_parameters = MemberParameters();
      daughter_parameters.member_id = daughter_id;
      daughter_parameters.character_id = 5;
      // daughter_parameters.speed = 80;
      daughter_parameters.speed = 100;
      daughter_parameters.name = name_generator.generate();
      spdlog::info("Daughters's name: {}", daughter_parameters.name);
      members.push_back(m_get_member_components(society, daughter_parameters));
    }
  }

  return members;
}

void SocietyGenerator::place_members(std::vector<MemberComponents>& components,
                                     const World& world,
                                     const Camera& camera,
                                     entt::registry& registry)
{
  using namespace entt::literals;

  for (auto& member : components)
  {
    auto entity = registry.create();
    registry.emplace<SocietyAgent>(entity, member.agent);
    registry.emplace<Biology>(entity, member.biology);
    registry.emplace<entt::tag<"collidable"_hs>>(entity, member.biology);

    const auto position = m_get_member_position(world, camera);
    registry.emplace<Position>(entity, position);
    member.visibility.layer_z = position.z + renderer::layer_z_offset_characters;
    registry.emplace<Visibility>(entity,
                                 member.visibility.resource_id,
                                 member.visibility.frame,
                                 member.visibility.layer_z,
                                 member.visibility.frame_angle);
    registry.emplace<CarriedItems>(entity, member.carried_items);
    registry.emplace<WearedItems>(entity, member.weared_items);
    registry.emplace<WieldedItems>(entity, member.wielded_items);
    registry.emplace<Selectable>(entity);
  }
}

Position SocietyGenerator::m_get_member_position(const World& world, const Camera& camera)
{
  auto position = Position{0., 0., 0.};
  const uint32_t max_tries = 50;

  for (uint32_t tries = 0; tries < max_tries; ++tries)
  {
    const auto x = camera.center_in_tiles.x + random::get_integer(-10, 10);
    const auto y = 35 + camera.center_in_tiles.y + random::get_integer(-10, 10);

    const auto elevation = world.get_elevation(x, y);

    if (world.is_walkable(x, y, elevation))
    {
      position.x = x;
      position.y = y;
      position.z = elevation;

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

  auto biology = Biology{member->sex, parameters.speed};

  // Defined in body_parts.json
  biology.body_parts = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

  return SocietyGenerator::MemberComponents{
      agent,
      biology,
      Visibility{"spritesheet-characters"_hs, parameters.character_id, 0, FrameAngle::Orthogonal},
      carried_items,
      weared_items,
      wielded_items};
}

}  // namespace dl
