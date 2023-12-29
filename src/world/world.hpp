#pragma once

#include <entt/entity/entity.hpp>
#include <map>
#include <vector>

#include "./chunk_manager.hpp"
#include "./grid_3d.hpp"
#include "./item_data.hpp"
#include "./society/society_blueprint.hpp"
#include "./spatial_hash.hpp"
#include "./tile_data.hpp"
#include "core/json.hpp"
#include "core/maths/vector.hpp"

namespace dl
{
struct GameContext;

enum class Direction
{
  Center,
  Top,
  Right,
  Bottom,
  Left,
  TopLeft,
  TopRight,
  BottomRight,
  BottomLeft,
};

struct TileTarget
{
  TileTarget(uint32_t id, int x, int y, int z) : id(id), x(x), y(y), z(z) {}
  TileTarget() : id(0), x(0), y(0), z(0) {}

  uint32_t id;
  int x, y, z;
  std::stack<std::pair<int, int>> path{};

  explicit operator bool() { return id != 0; }
};

struct WorldTile
{
  const TileData& terrain;
  const TileData& over_terrain;
};

class World
{
 public:
  // Spatial hash for nearby entities search
  SpatialHash spatial_hash;
  /* Grid3D tiles{}; */
  /* Grid3D over_tiles{}; */
  ChunkManager chunk_manager{};
  std::unordered_map<uint32_t, TileData> tile_data;
  std::unordered_map<uint32_t, ItemData> item_data;

  // Constructor
  World(GameContext& game_context);

  // Generate world
  void generate(const int width, const int height, const int seed);

  // Load map from a json file
  void load(const std::string& filepath);

  // Set tile by coordinates
  void set_terrain(const uint32_t tile_id, const int x, const int y, const int z);
  void set_over_terrain(const uint32_t tile_id, const int x, const int y, const int z);

  // Replace tile by coordinates
  void replace(const uint32_t from, const uint32_t to, const int x, const int y, const int z);

  // Get tile id by coordinates
  [[nodiscard]] uint32_t id_at(const int x, const int y, const int z) const;

  // Get tile data by coordinates
  [[nodiscard]] const TileData& get(const int x, const int y, const int z) const;

  // Get all tiles in a tile map coordinate
  [[nodiscard]] const WorldTile get_all(const int x, const int y, const int z) const;

  // Get terrain tile in a tile map coordinate
  [[nodiscard]] const TileData& get_terrain(const int x, const int y, const int z) const;

  // Get over terrain tile in a tile map coordinate
  [[nodiscard]] const TileData& get_over_terrain(const int x, const int y, const int z) const;

  // Gets the z elevation for a given (x, z) position
  [[nodiscard]] int get_elevation(const int x, const int y) const;

  // Get size of the tiles in the current tileset
  [[nodiscard]] Vector2i get_tile_size() { return m_tile_size; }

  // Get the seed used in this world generation
  [[nodiscard]] int get_seed() const { return m_seed; };

  // Get the size of a loaded chunk
  [[nodiscard]] size_t get_chunk_size() const { return m_chunk_size; };

  // Get the texture id for the tiles used in this world
  [[nodiscard]] const std::string& get_texture_id() const { return m_texture_id; };

  // Get a specific society
  [[nodiscard]] SocietyBlueprint get_society(const uint32_t society_id) const
  {
    assert(m_societies.contains(society_id));
    return m_societies.at(society_id);
  };

  // Get a path between two points
  [[nodiscard]] std::stack<std::pair<int, int>> get_path_between(const Vector3i& from, const Vector3i& to);

  // Get a nearby tile containing a flag
  [[nodiscard]] TileTarget search_by_flag(const std::string& flag, const int x, const int y, const int z) const;

  // Check if a specific tile is adjacent to a position
  [[nodiscard]] bool adjacent(const uint32_t tile_id, const int x, const int y, const int z) const;

  // Check if a specific tile is has WALKABLE flag
  [[nodiscard]] bool is_walkable(const int x, const int y, const int z) const;

  // Check if the world has a specific pattern for a given position
  [[nodiscard]] bool has_pattern(const std::vector<uint32_t>& pattern,
                                 const Vector2i& size,
                                 const Vector3i& position) const;

  // Get information about a tile with id
  [[nodiscard]] const TileData& get_tile_data(const uint32_t id) const;

  // Get information about an item with id
  [[nodiscard]] const ItemData& get_item_data(const uint32_t id) const;

  // Create a item and add it to the world
  entt::entity create_item(entt::registry& registry, const uint32_t id, const int x, const int y, const int z) const;

  template <class Archive>
  void serialize(Archive& archive)
  {
    archive(m_seed, m_societies);
  }

 private:
  GameContext& m_game_context;
  JSON m_json{"./data/world.json"};
  size_t m_chunk_size = 0;
  std::string m_texture_id;
  int m_seed = 0;
  Vector2i m_tile_size{0, 0};
  std::map<uint32_t, SocietyBlueprint> m_societies;

  // Load information about tiles
  void m_load_tile_data();

  // Load information about items
  void m_load_item_data();
};
}  // namespace dl
