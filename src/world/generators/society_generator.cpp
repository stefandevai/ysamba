#include "./society_generator.hpp"

#include "../../ecs/components/position.hpp"
#include "../../ecs/components/visibility.hpp"

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

  void SocietyGenerator::generate_members(const int seed, const Society& society, entt::registry& registry)
  const {
    const auto member1 = registry.create();
    registry.emplace<Position>(member1, 0., 0., 0.);
    registry.emplace<Visibility>(member1, 'A', 255, 255, 255);

    const auto member2 = registry.create();
    registry.emplace<Position>(member2, 3., 3., 0.);
    registry.emplace<Visibility>(member2, 'A', 255, 255, 255);
  }
}

