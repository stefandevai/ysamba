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
    const auto member1 = registry.create();
    registry.emplace<SocietyAgent>(member1, "adam", "otomi", "Adam", SocialClass::None, Metier::None);
    registry.emplace<Biology>(member1, 7305, 175, 80000, Sex::Male, "");
    registry.emplace<Position>(member1, 0., 0., 0.);
    registry.emplace<Visibility>(member1, "spritesheet-tileset", 24);
    registry.emplace<Velocity>(member1, 1.3, 0.7, 0.);

    const auto member2 = registry.create();
    registry.emplace<SocietyAgent>(member2, "eve", "otomi", "Eve", SocialClass::None, Metier::None);
    registry.emplace<Biology>(member2, 7305, 164, 70000, Sex::Female, "");
    registry.emplace<Position>(member2, 3., 3., 0.);
    registry.emplace<Visibility>(member2, "spritesheet-tileset", 24);
    registry.emplace<Velocity>(member2, 1., 0.2, 0.);
  }
}

