#include "./society_generator.hpp"

#include <entt/entity/registry.hpp>

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
  (void)seed;

  Society society{};

  society.id = "otomi";
  society.name = "Otomi";
  society.age = 1000;
  society.predominance = Predominance::Patriarchy;
  society.type = SocietyType::Sedentary;
  society.mode_of_production = ModeOfProduction::PrimitiveCommunism;

  return society;
}

void SocietyGenerator::generate_members(const int seed, const Society& society, entt::registry& registry) const
{
  (void)seed;
  (void)society;

  const auto member1 = registry.create();
  registry.emplace<SocietyAgent>(member1, "adam", "otomi", "Adam", SocialClass::None, Metier::None);
  registry.emplace<Biology>(member1, Sex::Male, 100);
  registry.emplace<Position>(member1, 8., 8., 0.);
  registry.emplace<Visibility>(member1, "spritesheet-characters", 0);
  registry.emplace<Selectable>(member1);

  const auto member2 = registry.create();
  registry.emplace<SocietyAgent>(member2, "adam", "otomi", "Adam", SocialClass::None, Metier::None);
  registry.emplace<Biology>(member2, Sex::Male, 0);
  registry.emplace<Position>(member2, 9., 8., 0.);
  registry.emplace<Visibility>(member2, "spritesheet-characters", 2);
  registry.emplace<Selectable>(member2);

  /* const uint32_t number_of_members = 10; */

  /* for (uint32_t i = 0; i < number_of_members; ++i) */
  /* { */
  /*   const auto member = registry.create(); */
  /*   registry.emplace<SocietyAgent>(member, "adam", "otomi", "Adam", SocialClass::None, Metier::None); */
  /*   registry.emplace<Biology>(member, Sex::Male, 100.); */
  /*   registry.emplace<Position>(member, i, 8., 0.); */
  /*   registry.emplace<Visibility>(member, "spritesheet-characters", i % 6); */
  /*   registry.emplace<Selectable>(member); */
  /* } */
  /* for (uint32_t i = 0; i < number_of_members; ++i) */
  /* { */
  /*   const auto member = registry.create(); */
  /*   registry.emplace<SocietyAgent>(member, "adam", "otomi", "Adam", SocialClass::None, Metier::None); */
  /*   registry.emplace<Biology>(member, Sex::Male, 100.); */
  /*   registry.emplace<Position>(member, i, 9., 0.); */
  /*   registry.emplace<Visibility>(member, "spritesheet-characters", i % 6); */
  /*   registry.emplace<Selectable>(member); */
  /* } */
  /* for (uint32_t i = 0; i < number_of_members; ++i) */
  /* { */
  /*   const auto member = registry.create(); */
  /*   registry.emplace<SocietyAgent>(member, "adam", "otomi", "Adam", SocialClass::None, Metier::None); */
  /*   registry.emplace<Biology>(member, Sex::Male, 100.); */
  /*   registry.emplace<Position>(member, i, 10., 0.); */
  /*   registry.emplace<Visibility>(member, "spritesheet-characters", i % 6); */
  /*   registry.emplace<Selectable>(member); */
  /* } */
  /* for (uint32_t i = 0; i < number_of_members; ++i) */
  /* { */
  /*   const auto member = registry.create(); */
  /*   registry.emplace<SocietyAgent>(member, "adam", "otomi", "Adam", SocialClass::None, Metier::None); */
  /*   registry.emplace<Biology>(member, Sex::Male, 100.); */
  /*   registry.emplace<Position>(member, i, 11., 0.); */
  /*   registry.emplace<Visibility>(member, "spritesheet-characters", i % 6); */
  /*   registry.emplace<Selectable>(member); */
  /* } */
  /* for (uint32_t i = 0; i < number_of_members; ++i) */
  /* { */
  /*   const auto member = registry.create(); */
  /*   registry.emplace<SocietyAgent>(member, "adam", "otomi", "Adam", SocialClass::None, Metier::None); */
  /*   registry.emplace<Biology>(member, Sex::Male, 100.); */
  /*   registry.emplace<Position>(member, i, 12., 0.); */
  /*   registry.emplace<Visibility>(member, "spritesheet-characters", i % 6); */
  /*   registry.emplace<Selectable>(member); */
  /* } */
}
}  // namespace dl
