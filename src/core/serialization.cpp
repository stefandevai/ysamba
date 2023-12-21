#include "./serialization.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>
#include <fstream>

#include "ecs/components/action_pickup.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/job_progress.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/visibility.hpp"
#include "ecs/components/walk_path.hpp"
#include "world/world.hpp"

namespace dl::serialization
{
void save_world(World& world)
{
  std::ofstream output{"world.dl"};
  cereal::BinaryOutputArchive archive{output};
  archive(world);
}

void load_world(World& world)
{
  std::ifstream input{"world.dl"};
  cereal::BinaryInputArchive archive{input};
  archive(world);
}

void save_game(World& world, entt::registry& registry)
{
  save_world(world);

  std::ofstream output{"game.dl"};
  cereal::BinaryOutputArchive archive{output};

  entt::snapshot{registry}
      .get<entt::entity>(archive)
      .get<Position>(archive)
      .get<Velocity>(archive)
      .get<Biology>(archive)
      .get<CarriedItems>(archive)
      .get<WalkPath>(archive)
      .get<Visibility>(archive)
      .get<SocietyAgent>(archive)
      .get<Selectable>(archive)
      .get<Item>(archive)
      .get<JobProgress>(archive);
}

void load_game(World& world, entt::registry& registry)
{
  load_world(world);

  std::ifstream input{"game.dl"};
  cereal::BinaryInputArchive archive{input};

  entt::snapshot_loader{registry}
      .get<entt::entity>(archive)
      .get<Position>(archive)
      .get<Velocity>(archive)
      .get<Biology>(archive)
      .get<CarriedItems>(archive)
      .get<WalkPath>(archive)
      .get<Visibility>(archive)
      .get<SocietyAgent>(archive)
      .get<Selectable>(archive)
      .get<Item>(archive)
      .get<JobProgress>(archive);
}
}  // namespace dl::serialization
