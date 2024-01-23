#include "./serialization.hpp"

#include <spdlog/spdlog.h>

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
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "world/grid_3d.hpp"
#include "world/world.hpp"

namespace dl::serialization
{
namespace terrain_ext
{
constexpr uint32_t magic_number = 0x533d;
constexpr uint8_t metadata_marker = 0x01;
constexpr uint8_t values_marker = 0x02;
constexpr uint8_t height_map_marker = 0x03;
constexpr uint8_t end_marker = 0x04;

constexpr size_t marker_size = sizeof(uint8_t);
constexpr size_t cell_size = sizeof(uint32_t) * 3;
}  // namespace terrain_ext

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
      .get<WearedItems>(archive)
      .get<WieldedItems>(archive)
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
      .get<WearedItems>(archive)
      .get<WieldedItems>(archive)
      .get<WalkPath>(archive)
      .get<Visibility>(archive)
      .get<SocietyAgent>(archive)
      .get<Selectable>(archive)
      .get<Item>(archive)
      .get<JobProgress>(archive);
}

void save_terrain(const Grid3D& tiles, const std::string& file_name)
{
  std::ofstream outfile{file_name, std::ios::binary | std::ios::out};
  if (!outfile.is_open())
  {
    spdlog::debug("Could not open file to save world");
    return;
  }

  outfile.write(reinterpret_cast<const char*>(&terrain_ext::magic_number), sizeof(terrain_ext::magic_number));

  outfile.write(reinterpret_cast<const char*>(&terrain_ext::metadata_marker), sizeof(terrain_ext::metadata_marker));

  outfile.write(reinterpret_cast<const char*>(&tiles.size.x), sizeof(tiles.size.x));
  outfile.write(reinterpret_cast<const char*>(&tiles.size.y), sizeof(tiles.size.y));
  outfile.write(reinterpret_cast<const char*>(&tiles.size.z), sizeof(tiles.size.z));

  outfile.write(reinterpret_cast<const char*>(&terrain_ext::values_marker), sizeof(terrain_ext::values_marker));

  for (const auto& cell : tiles.values)
  {
    outfile.write(reinterpret_cast<const char*>(&cell.terrain), sizeof(cell.terrain));
    outfile.write(reinterpret_cast<const char*>(&cell.decoration), sizeof(cell.decoration));
    outfile.write(reinterpret_cast<const char*>(&cell.flags), sizeof(cell.flags));
  }

  outfile.write(reinterpret_cast<const char*>(&terrain_ext::height_map_marker), sizeof(terrain_ext::height_map_marker));

  for (const int value : tiles.height_map)
  {
    outfile.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

  outfile.write(reinterpret_cast<const char*>(&terrain_ext::end_marker), sizeof(terrain_ext::end_marker));

  outfile.close();
}

void load_chunk(Chunk& chunk, const std::string& file_name)
{
  const auto start1 = std::chrono::high_resolution_clock::now();

  const auto& position = chunk.position;
  auto& tiles = chunk.tiles;

  uint32_t file_magic_number = 0;
  uint8_t file_metadata_marker = 0;
  uint8_t file_values_marker = 0;
  uint8_t file_height_map_marker = 0;

  FILE* file = fopen(file_name.c_str(), "r");

  if (!file)
  {
    spdlog::critical("Could not open file for loading world.");
    return;
  }

  fread(&file_magic_number, sizeof(uint32_t), 1, file);

  if (file_magic_number != terrain_ext::magic_number)
  {
    spdlog::critical(
        "Invalid file format when loading world: {0:x}, expected: {0:x}", file_magic_number, terrain_ext::magic_number);
    fclose(file);
    return;
  }

  fread(&file_metadata_marker, terrain_ext::marker_size, 1, file);

  if (file_metadata_marker != terrain_ext::metadata_marker)
  {
    spdlog::critical("Invalid metadata marker when loading world: {}, expected: {}",
                     file_metadata_marker,
                     terrain_ext::metadata_marker);
    fclose(file);
    return;
  }

  Vector3i world_size{};

  fread(&world_size.x, sizeof(uint32_t), 1, file);
  fread(&world_size.y, sizeof(uint32_t), 1, file);
  fread(&world_size.z, sizeof(uint32_t), 1, file);

  const auto stop1 = std::chrono::high_resolution_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);

  const auto start2 = std::chrono::high_resolution_clock::now();

  // spdlog::debug("World size: {} {} {}", world_size.x, world_size.y, world_size.z);

  fread(&file_values_marker, terrain_ext::marker_size, 1, file);

  if (file_values_marker != terrain_ext::values_marker)
  {
    spdlog::critical(
        "Invalid metadata marker when loading world: {}, expected: {}", file_values_marker, terrain_ext::values_marker);
    fclose(file);
    return;
  }

  const uint64_t values_pos = ftell(file);
  const uint64_t max_cell_pos = world_size.x * world_size.y * world_size.z * terrain_ext::cell_size;

  const uint64_t local_position =
      ((position.x) + (position.y + 0) * world_size.x + (position.z + 0) * world_size.x * world_size.y) *
      terrain_ext::cell_size;

  fseek(file, local_position, SEEK_CUR);

  for (int z = 0; z < tiles.size.z; ++z)
  {
    if (z > 0)
    {
      const auto offset_advanced = (tiles.size.y - 1) * world_size.x + tiles.size.x;
      fseek(file, (world_size.x * world_size.y - offset_advanced) * terrain_ext::cell_size, SEEK_CUR);
    }

    for (int y = 0; y < tiles.size.y; ++y)
    {
      if (y > 0)
      {
        fseek(file, (world_size.x - tiles.size.x) * terrain_ext::cell_size, SEEK_CUR);
      }

      for (int x = 0; x < tiles.size.x; ++x)
      {
        auto& cell = tiles.values[x + y * tiles.size.x + z * tiles.size.x * tiles.size.y];
        fread(&cell.terrain, sizeof(uint32_t), 1, file);
        fread(&cell.decoration, sizeof(uint32_t), 1, file);
        fread(&cell.flags, sizeof(uint32_t), 1, file);
      }
    }
  }

  const auto stop2 = std::chrono::high_resolution_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2);
  const auto start3 = std::chrono::high_resolution_clock::now();

  const auto pos_offset = ftell(file) - values_pos;

  fseek(file, max_cell_pos - pos_offset, SEEK_CUR);
  fread(&file_height_map_marker, terrain_ext::marker_size, 1, file);

  if (file_height_map_marker != terrain_ext::height_map_marker)
  {
    spdlog::critical("Invalid metadata marker when loading world: {}, expected: {}",
                     file_height_map_marker,
                     terrain_ext::height_map_marker);
    fclose(file);
    return;
  }

  const uint64_t local_position2 = ((position.x + 0) + (position.y + 0) * world_size.x) * sizeof(int);

  fseek(file, local_position2, SEEK_CUR);

  for (int y = 0; y < tiles.size.y; ++y)
  {
    if (y > 0)
    {
      fseek(file, (world_size.x - tiles.size.x) * sizeof(int), SEEK_CUR);
    }

    for (int x = 0; x < tiles.size.x; ++x)
    {
      fread(&tiles.height_map[x + y * tiles.size.x], sizeof(int), 1, file);
    }
  }

  const auto stop3 = std::chrono::high_resolution_clock::now();
  auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(stop3 - start3);

  spdlog::info("INSIDE: metadata: {} ms, cells: {} ms, height_map {} ms",
               duration1.count(),
               duration2.count(),
               duration3.count());

  fclose(file);
}
}  // namespace dl::serialization
