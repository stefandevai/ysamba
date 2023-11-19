#include "./society_generator.hpp"

#include "../../ecs/components/position.hpp"
#include "../../ecs/components/visibility.hpp"
#include "../../ecs/components/biology.hpp"
#include "../../ecs/components/society_agent.hpp"
#include "../../ecs/components/velocity.hpp"

namespace dl
{
  SocietyGenerator::SocietyGenerator()
  {

  }

  Society SocietyGenerator::generate(const int seed)
  {
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
    const uint32_t number_of_members = 30;

    for (uint32_t i = 0; i < number_of_members; ++i)
    {
      const auto member = registry.create();
      registry.emplace<SocietyAgent>(member, "adam", "otomi", "Adam", SocialClass::None, Metier::None);
      registry.emplace<Biology>(member, Sex::Male, 100.);
      registry.emplace<Position>(member, i, 10., 0.);
      registry.emplace<Visibility>(member, "spritesheet-tileset", 4);
    }

    /* const auto member1 = registry.create(); */
    /* registry.emplace<SocietyAgent>(member1, "adam", "otomi", "Adam", SocialClass::None, Metier::None); */
    /* registry.emplace<Biology>(member1, Sex::Male, 100.); */
    /* registry.emplace<Position>(member1, 0., 0., 0.); */
    /* registry.emplace<Visibility>(member1, "spritesheet-tileset", 4); */

    /* const auto member2 = registry.create(); */
    /* registry.emplace<SocietyAgent>(member2, "eve", "otomi", "Eve", SocialClass::None, Metier::None); */
    /* registry.emplace<Biology>(member2, Sex::Female, 100.); */
    /* registry.emplace<Position>(member2, 3., 3., 0.); */
    /* registry.emplace<Visibility>(member2, "spritesheet-tileset", 4); */
  }
}

