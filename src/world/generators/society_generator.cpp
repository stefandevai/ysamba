#include "./society_generator.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/registry.hpp>

#include "../../core/random.hpp"
#include "../../ecs/components/biology.hpp"
#include "../../ecs/components/position.hpp"
#include "../../ecs/components/selectable.hpp"
#include "../../ecs/components/society_agent.hpp"
#include "../../ecs/components/velocity.hpp"
#include "../../ecs/components/visibility.hpp"

namespace dl
{
SocietyGenerator::SocietyGenerator() {}

Society SocietyGenerator::generate(const int seed)
{
  random::rng.seed(seed);

  Society society{};

  society.id = "otomi";
  society.name = "Otomi";
  society.age = random::get_integer(148, 1848);
  society.predominance = static_cast<Predominance>(random::get_integer(0, 2));
  society.type = static_cast<SocietyType>(random::get_integer(0, 2));
  society.mode_of_production = ModeOfProduction::PrimitiveCommunism;

  return society;
}

void SocietyGenerator::generate_members(const int seed, Society& society, entt::registry& registry)
{
  random::rng.seed(seed);

  switch (society.mode_of_production)
  {
  case ModeOfProduction::PrimitiveCommunism:
    m_generate_primitive_communism_members(society, registry);
    break;
  case ModeOfProduction::Slavery:
    /* m_generate_slavery_members(society, registry); */
    break;
  default:
    break;
  }

  /* if (society.mode_of_production == ModeOfProduction::PrimitiveCommunism) */
  /* { */
  /*   genera */
  /* } */

  /* const auto member1 = registry.create(); */
  /* registry.emplace<SocietyAgent>(member1, "adam", "otomi", "Adam", SocialClass::None, Metier::None); */
  /* registry.emplace<Biology>(member1, Sex::Male, 100); */
  /* registry.emplace<Position>(member1, 9., 16., 0.); */
  /* registry.emplace<Visibility>(member1, "spritesheet-characters", 0); */
  /* registry.emplace<Selectable>(member1); */

  /* const auto member2 = registry.create(); */
  /* registry.emplace<SocietyAgent>(member2, "adam", "otomi", "Adam", SocialClass::None, Metier::None); */
  /* registry.emplace<Biology>(member2, Sex::Male, 80); */
  /* registry.emplace<Position>(member2, 9., 15., 0.); */
  /* registry.emplace<Visibility>(member2, "spritesheet-characters", 2); */
  /* registry.emplace<Selectable>(member2); */

  /* const uint32_t number_of_members = 10; */

  /* for (uint32_t i = 0; i < number_of_members; ++i) */
  /* { */
  /*   const auto member = registry.create(); */
  /*   registry.emplace<SocietyAgent>(member, "adam", "otomi", "Adam", SocialClass::None, Metier::None); */
  /*   registry.emplace<Biology>(member, Sex::Male, 100.); */
  /*   registry.emplace<Position>(member, i + 7, 8., 0.); */
  /*   registry.emplace<Visibility>(member, "spritesheet-characters", i % 6); */
  /*   registry.emplace<Selectable>(member); */
  /* } */
}

void SocietyGenerator::m_generate_primitive_communism_members(Society& society, entt::registry& registry)
{
  /* const auto first_generation_members = random::get_integer(1, 2); */
  const auto first_generation_members = 1;
  /* const auto cacique = registry.create(); */

  for (auto i = 0; i < first_generation_members; ++i)
  {
    const auto first_member_id = society.add_first_member(Sex::Male);
    const auto first_member_spouse_id = society.add_spouse(first_member_id);

    m_create_person_entity(registry, society, first_member_id);
    m_create_person_entity(registry, society, first_member_spouse_id);

    const auto number_of_sons = 1;

    for (auto j = 0; j < number_of_sons; ++j)
    {
      const auto son = society.add_son(first_member_id);
      m_create_person_entity(registry, society, son);
    }

    const auto number_of_daughters = 1;

    for (auto j = 0; j < number_of_daughters; ++j)
    {
      const auto daughter = society.add_daughter(first_member_id);
      m_create_person_entity(registry, society, daughter);
    }
  }
}

/* void SocietyGenerator::m_generate_slavery_members(const Society& society, entt::registry& registry) {} */

void SocietyGenerator::m_create_person_entity(entt::registry& registry,
                                              const Society& society,
                                              const uint32_t person_id)
{
  spdlog::info("PERSONID {}", person_id);
  const auto& person = society.get_member(person_id);
  const auto person_entity = registry.create();

  registry.emplace<SocietyAgent>(person_entity, "adam", "otomi", "Adam", SocialClass::None, Metier::None);

  registry.emplace<Biology>(person_entity, person->sex, 100);

  const auto x = random::get_integer(10, 20);
  const auto y = random::get_integer(10, 20);
  registry.emplace<Position>(person_entity, x, y, 0.);

  auto frame = 0;

  if (person->sex == Sex::Female)
  {
    frame = 1;
  }

  registry.emplace<Visibility>(person_entity, "spritesheet-characters", frame);
}

}  // namespace dl
