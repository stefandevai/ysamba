#include "./society_generator.hpp"

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

void SocietyGenerator::generate_members(const int seed, const Society& society, entt::registry& registry) const
{
  rng.seed(seed);

  switch (society.mode_of_production)
  {
  case ModeOfProduction::PrimitiveCommunism:
    m_generate_primitive_communism_members(society, registry);
    break;
  case ModeOfProduction::Slavery:
    m_generate_slavery_members(society, registry);
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

void SocietyGenerator::m_generate_primitive_communism_members(const Society& society, entt : registry& registry)
{
  const auto number_of_families = random::get_int(1, 6);
  const auto cacique = registry.create();

  const auto sex;

  if (society.predominance == Predominance::Patriarchy)
  {
    sex = Sex::Male;
  }
  else
  {
    sex = Sex::Female;
  }

  const auto member1 = registry.create();
  registry.emplace<SocietyAgent>(member1, "adam", "otomi", "Adam", SocialClass::None, Metier::None);
  registry.emplace<Biology>(member1, Sex::Male, 100);
  registry.emplace<Position>(member1, 9., 16., 0.);
  registry.emplace<Visibility>(member1, "spritesheet-characters", 0);
  registry.emplace<Selectable>(member1);
}

void SocietyGenerator::m_generate_slavery_members(const Society& society, entt : registry& registry) {}

}  // namespace dl
