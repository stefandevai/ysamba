#include "./serialization.hpp"

#ifdef _WIN32
#include <chrono>
#else
#include <date/date.h>
#endif
#include <fmt/chrono.h>
#include <spdlog/spdlog.h>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/snapshot.hpp>
#include <fstream>
#include <sstream>

#include "constants.hpp"
#include "ecs/components/action_pickup.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/job_progress.hpp"
#include "ecs/components/movement.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/sprite.hpp"
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

void initialize_directories()
{
  if (!std::filesystem::exists(directory::data))
  {
    std::filesystem::create_directory(directory::data);
  }
  else if (!std::filesystem::is_directory(directory::data))
  {
    spdlog::critical("{} is not a directory", directory::data.string());
  }

  if (!std::filesystem::exists(directory::worlds))
  {
    std::filesystem::create_directory(directory::worlds);
  }
  else if (!std::filesystem::is_directory(directory::worlds))
  {
    spdlog::critical("{} is not a directory", directory::worlds.string());
  }
}

void save_world_metadata(const WorldMetadata& metadata)
{
  const auto world_directory = directory::worlds / metadata.id;

  if (!std::filesystem::exists(world_directory))
  {
    std::filesystem::create_directory(world_directory);
  }

  std::ofstream output{world_directory / filename::metadata};
  cereal::JSONOutputArchive archive{output};

  WorldMetadata data = metadata;
  data.created_at_label = fmt::format("{:%Y-%m-%d %H:%M:%S}", data.created_at);
  data.updated_at_label = fmt::format("{:%Y-%m-%d %H:%M:%S}", data.updated_at);

  archive(data);
}

WorldMetadata load_world_metadata(const std::string& id)
{
  const auto world_directory = directory::worlds / id;

  if (!std::filesystem::exists(world_directory))
  {
    std::filesystem::create_directory(world_directory);
  }

  const auto metadata_path = world_directory / filename::metadata;

  if (!std::filesystem::exists(metadata_path))
  {
    return {};
  }

  std::ifstream input{metadata_path};
  cereal::JSONInputArchive archive{input};

  WorldMetadata metadata{};
  archive(metadata);

  std::istringstream created_ss{metadata.created_at_label.c_str()};
#ifdef _WIN32
  created_ss >> std::chrono::parse("%Y-%m-%d %H:%M:%S", metadata.created_at);
#else
  created_ss >> date::parse("%Y-%m-%d %H:%M:%S", metadata.created_at);
#endif

  std::istringstream updated_ss{metadata.updated_at_label.c_str()};
#ifdef _WIN32
  created_ss >> std::chrono::parse("%Y-%m-%d %H:%M:%S", metadata.created_at);
#else
  updated_ss >> date::parse("%Y-%m-%d %H:%M:%S", metadata.updated_at);
#endif

  return metadata;
}

void save_world(const World& world, const WorldMetadata& world_metadata)
{
  const auto full_path = directory::worlds / world_metadata.id / filename::game;
  std::ofstream output{full_path.c_str()};
  cereal::BinaryOutputArchive archive{output};
  archive(world);
}

void load_world(World& world, WorldMetadata& world_metadata)
{
  const auto full_path = directory::worlds / world_metadata.id / filename::game;
  std::ifstream input{full_path.c_str()};
  cereal::BinaryInputArchive archive{input};
  archive(world);
}

void save_game(World& world, const WorldMetadata& world_metadata, entt::registry& registry)
{
  using namespace entt::literals;

  const auto full_path = directory::worlds / world_metadata.id / filename::game;
  std::ofstream output{full_path.c_str()};
  cereal::BinaryOutputArchive archive{output};

  archive(world);

  entt::snapshot{registry}
      .get<entt::entity>(archive)
      .get<Position>(archive)
      .get<Movement>(archive)
      .get<Biology>(archive)
      .get<entt::tag<"collidable"_hs>>(archive)
      .get<CarriedItems>(archive)
      .get<WearedItems>(archive)
      .get<WieldedItems>(archive)
      .get<WalkPath>(archive)
      .get<Sprite>(archive)
      .get<SocietyAgent>(archive)
      .get<Selectable>(archive)
      .get<Item>(archive)
      .get<JobProgress>(archive);
}

void load_game(World& world, const WorldMetadata& world_metadata, entt::registry& registry)
{
  using namespace entt::literals;

  const auto full_path = directory::worlds / world_metadata.id / filename::game;
  std::ifstream input{full_path.c_str()};
  cereal::BinaryInputArchive archive{input};

  archive(world);

  if (!world.has_initialized)
  {
    return;
  }

  entt::snapshot_loader{registry}
      .get<entt::entity>(archive)
      .get<Position>(archive)
      .get<Movement>(archive)
      .get<Biology>(archive)
      .get<entt::tag<"collidable"_hs>>(archive)
      .get<CarriedItems>(archive)
      .get<WearedItems>(archive)
      .get<WieldedItems>(archive)
      .get<WalkPath>(archive)
      .get<Sprite>(archive)
      .get<SocietyAgent>(archive)
      .get<Selectable>(archive)
      .get<Item>(archive)
      .get<JobProgress>(archive);
}

bool chunk_exists(const Vector3i& position, const std::string& world_id)
{
  assert(position.x % world::chunk_size.x == 0 && position.y % world::chunk_size.y == 0
         && position.z % world::chunk_size.z == 0 && "Position is not a chunk position.");

  const auto filename = fmt::format("{}_{}_{}.chunk", position.x, position.y, position.z);
  const auto full_path = directory::worlds / world_id / directory::chunks / filename;
  return std::filesystem::exists(full_path);
}

void save_game_chunk(const Chunk& chunk, const std::string& world_id)
{
  const auto chunks_directory = directory::worlds / world_id / directory::chunks;

  if (!std::filesystem::exists(chunks_directory))
  {
    std::filesystem::create_directories(chunks_directory);
  }
  else if (!std::filesystem::is_directory(chunks_directory))
  {
    spdlog::critical("{} is not a directory", chunks_directory.string());
    return;
  }

  const auto filename = fmt::format("{}_{}_{}.chunk", chunk.position.x, chunk.position.y, chunk.position.z);
  const auto full_path = chunks_directory / filename;
  const auto& tiles = chunk.tiles;

  std::ofstream outfile{full_path.c_str(), std::ios::binary | std::ios::out};

  if (!outfile.is_open())
  {
    spdlog::critical("Could not open file to save chunk: {}", full_path.string());
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

void load_game_chunk(Chunk& chunk, const std::string& world_id)
{
  const auto filename = fmt::format("{}_{}_{}.chunk", chunk.position.x, chunk.position.y, chunk.position.z);
  const auto full_path = directory::worlds / world_id / directory::chunks / filename;

  if (!std::filesystem::exists(full_path))
  {
    spdlog::critical("Chunk file doest not does not exist: {}", filename.c_str());
    return;
  }

  // Timer timer1{};
  // Timer timer2{};
  // Timer timer3{};
  // timer1.start();

#ifdef _WIN32
  FILE* file = _wfopen(full_path.c_str(), L"r");
#else
  FILE* file = fopen(full_path.c_str(), "r");
#endif

  auto& tiles = chunk.tiles;

  uint32_t file_magic_number = 0;
  uint8_t file_metadata_marker = 0;
  uint8_t file_values_marker = 0;
  uint8_t file_height_map_marker = 0;

  if (!file)
  {
    spdlog::critical("Could not open file for loading chunk.");
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
    spdlog::critical("Invalid metadata marker when loading chunk: {}, expected: {}",
                     file_metadata_marker,
                     terrain_ext::metadata_marker);
    fclose(file);
    return;
  }

  Vector3i world_size{};

  fread(&world_size.x, sizeof(uint32_t), 3, file);

  // timer1.stop();
  // timer2.start();

  // spdlog::debug("World size: {} {} {}", world_size.x, world_size.y, world_size.z);

  assert(world_size.x >= tiles.size.x && world_size.y >= tiles.size.y && world_size.z >= tiles.size.z
         && "Chunk size is bigger than world size.");

  fread(&file_values_marker, terrain_ext::marker_size, 1, file);

  if (file_values_marker != terrain_ext::values_marker)
  {
    spdlog::critical(
        "Invalid metadata marker when loading chunk: {}, expected: {}", file_values_marker, terrain_ext::values_marker);
    fclose(file);
    return;
  }

  const uint64_t values_pos = ftell(file);
  const uint64_t max_cell_pos = world_size.x * world_size.y * world_size.z * terrain_ext::cell_size;

  // const auto& position = chunk.position;
  // const uint64_t local_position =
  //     ((position.x) + (position.y + 0) * world_size.x + (position.z + 0) * world_size.x * world_size.y) *
  //     terrain_ext::cell_size;
  //
  // fseek(file, local_position, SEEK_CUR);

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
        fread(&cell.terrain, sizeof(uint32_t), 3, file);
      }
    }
  }

  // timer2.stop();
  // timer3.start();

  const auto pos_offset = ftell(file) - values_pos;

  fseek(file, max_cell_pos - pos_offset, SEEK_CUR);
  fread(&file_height_map_marker, terrain_ext::marker_size, 1, file);

  if (file_height_map_marker != terrain_ext::height_map_marker)
  {
    spdlog::critical("Invalid metadata marker when loading chunk: {}, expected: {}",
                     file_height_map_marker,
                     terrain_ext::height_map_marker);
    fclose(file);
    return;
  }

  // const uint64_t local_position2 = ((0 + 0) + (0 + 0) * world_size.x) * sizeof(int);
  //
  // fseek(file, local_position2, SEEK_CUR);

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

  // timer3.stop();
  // timer1.print("metadata");
  // timer2.print("cells");
  // timer3.print("height_map");

  fclose(file);
}
}  // namespace dl::serialization
