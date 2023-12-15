#include "./society_generator.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include "./name_generator.hpp"
#include "core/random.hpp"
#include "ecs/components/selectable.hpp"
#include "graphics/camera.hpp"
#include "graphics/constants.hpp"
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
    father_parameters.texture_frame = 0;
    father_parameters.speed = 100;
    father_parameters.name = name_generator.generate();
    members.push_back(m_get_member_components(society, father_parameters));

    const auto mother_id = society.add_spouse(father_id);
    auto mother_parameters = MemberParameters();
    mother_parameters.member_id = mother_id;
    mother_parameters.texture_frame = 1;
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
      son_parameters.texture_frame = 4;
      son_parameters.speed = 80;
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
      daughter_parameters.texture_frame = 5;
      daughter_parameters.speed = 80;
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
  for (auto& member : components)
  {
    auto entity = registry.create();
    registry.emplace<SocietyAgent>(entity, member.agent);
    registry.emplace<Biology>(entity, member.biology);

    const auto position = m_get_member_position(world, camera);
    registry.emplace<Position>(entity, position);
    member.visibility.layer_z = position.z + renderer::layer_z_offset_characters;
    registry.emplace<Visibility>(
        entity, member.visibility.resource_id, member.visibility.frame, member.visibility.layer_z);
    registry.emplace<CarriedItems>(entity, member.carried_items);
    registry.emplace<WearedItems>(entity, member.weared_items);
    registry.emplace<Selectable>(entity);
  }
}

Position SocietyGenerator::m_get_member_position(const World& world, const Camera& camera)
{
  const auto& camera_position = camera.get_position_in_tiles();
  const auto& camera_size = camera.get_size_in_tiles();
  const auto placement_margin = 5;

  auto position = Position{0., 0., 0.};
  const uint32_t max_tries = 10;

  for (uint32_t tries = 0; tries < max_tries; ++tries)
  {
    const auto x = static_cast<double>(random::get_integer(camera_position.x + placement_margin,
                                                           camera_position.x + camera_size.x - placement_margin));
    const auto y = static_cast<double>(random::get_integer(camera_position.y + placement_margin,
                                                           camera_position.y + camera_size.y - placement_margin));

    if (world.is_walkable(x, y, 0.))
    {
      position.x = x;
      position.y = y;

      return position;
    }
  }

  spdlog::warn("Could not find position to place society member");

  return position;
}

SocietyGenerator::MemberComponents SocietyGenerator::m_get_member_components(const SocietyBlueprint& society,
                                                                             const MemberParameters& parameters)
{
  const auto& member = society.get_member(parameters.member_id);
  const auto agent = SocietyAgent{parameters.member_id, society.id, parameters.name, SocialClass::None, Metier::None};
  const auto carried_items = CarriedItems{};
  const auto weared_items = WearedItems{};

  auto biology = Biology{member->sex, parameters.speed};

  // Defined in body_parts.json
  biology.body_parts = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

  return SocietyGenerator::MemberComponents{
      agent, biology, Visibility{"spritesheet-characters", parameters.texture_frame, 0}, carried_items, weared_items};
}

}  // namespace dl
