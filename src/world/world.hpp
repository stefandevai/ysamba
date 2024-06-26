#pragma once

#include <entt/entity/entity.hpp>
#include <map>
#include <stack>
#include <vector>

#include "./chunk_manager.hpp"
#include "./grid_3d.hpp"
#include "./item_data.hpp"
#include "./society/society_blueprint.hpp"
#include "./spatial_hash.hpp"
#include "./tile_data.hpp"
#include "core/maths/vector.hpp"

namespace dl
{
struct GameContext;
struct Cell;
class Camera;

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
  TileTarget() = default;
  explicit TileTarget(Vector3i position) : position(std::move(position)) {}

  Vector3i position{};
  std::stack<Vector3i> path{};
};

struct WorldTile
{
  const TileData& top_face;
  const TileData& top_face_decoration;
};

class World
{
 private:
  GameContext& m_game_context;

 public:
  // Spatial hash for nearby entities search
  SpatialHash spatial_hash;
  ChunkManager chunk_manager{m_game_context};
  std::unordered_map<uint32_t, TileData> tile_data;
  std::unordered_map<uint32_t, ItemData> item_data;
  bool has_initialized = false;

  // Constructor
  World(GameContext& game_context);

  // Generate elements for a new world
  void initialize(entt::registry& registry);

  // Generate all societies for the world
  void generate_societies();

  // Set tile by coordinates
  void set_top_face(const uint32_t tile_id, const int x, const int y, const int z);
  void set_top_face_decoration(const uint32_t tile_id, const int x, const int y, const int z);

  // Replace tile by coordinates
  void replace(const uint32_t from, const uint32_t to, const int x, const int y, const int z);

  // Get cell by coordinates
  [[nodiscard]] const Cell& cell_at(const int x, const int y, const int z) const;
  [[nodiscard]] const Cell& cell_at(const Vector3i& position) const;

  // Get tile id by coordinates
  [[nodiscard]] uint32_t top_face_at(const int x, const int y, const int z) const;
  [[nodiscard]] uint32_t top_face_at(const Vector3i& position) const;

  // Get tile top_face_decoration by coordinates
  [[nodiscard]] uint32_t top_face_decoration_at(const int x, const int y, const int z) const;
  [[nodiscard]] uint32_t top_face_decoration_at(const Vector3i& position) const;

  // Get tile data by coordinates
  [[nodiscard]] const TileData& get(const int x, const int y, const int z) const;
  [[nodiscard]] const TileData& get(const Vector3i& position) const;

  // Get all tiles in a tile map coordinate
  [[nodiscard]] const WorldTile get_all(const int x, const int y, const int z) const;
  [[nodiscard]] const WorldTile get_all(const Vector3i& position) const;

  // Get top_face tile in a tile map coordinate
  [[nodiscard]] const TileData& get_top_face(const int x, const int y, const int z) const;
  [[nodiscard]] const TileData& get_top_face(const Vector3i& position) const;

  // Get over top_face tile in a tile map coordinate
  [[nodiscard]] const TileData& get_top_face_decoration(const int x, const int y, const int z) const;
  [[nodiscard]] const TileData& get_top_face_decoration(const Vector3i& position) const;

  // Gets the z elevation for a given (x, z) position
  [[nodiscard]] int get_elevation(const int x, const int y) const;
  [[nodiscard]] int get_elevation(const Vector2i& position) const;

  // Get size of the tiles in the current tileset
  [[nodiscard]] const Vector2i& get_tile_size() { return m_tile_size; }

  // Get the seed used in this world generation
  [[nodiscard]] int get_seed() const { return m_seed; };

  // Transform screen coordinates to world coordinates
  [[nodiscard]] Vector3i screen_to_world(const Vector2i& position, const Camera& camera) const;

  // Transform mouse coordinates to world coordinates
  [[nodiscard]] Vector3i mouse_to_world(const Camera& camera) const;

  // Get the spritesheet id for the tiles used in this world
  [[nodiscard]] uint32_t get_spritesheet_id() const { return m_spritesheet_id; };

  // Get a specific society
  [[nodiscard]] SocietyBlueprint get_society(const uint32_t society_id) const
  {
    assert(m_societies.contains(society_id));
    return m_societies.at(society_id);
  };

  // Find path between two points
  [[nodiscard]] std::vector<Vector3i> find_path(const Vector3i& from, const Vector3i& to);

  // Get a nearby tile containing a flag
  [[nodiscard]] TileTarget search_by_flag(const std::string& flag, const Vector3i& start) const;

  // Check if a specific tile is adjacent to a position
  [[nodiscard]] bool adjacent(const uint32_t tile_id, const int x, const int y, const int z) const;

  // Check if a specific tile is has WALKABLE flag
  [[nodiscard]] bool is_walkable(const int x, const int y, const int z) const;

  // Check if a specific tile is empty
  [[nodiscard]] bool is_empty(const int x, const int y, const int z) const;

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
    archive(m_societies, has_initialized);
  }

 private:
  uint32_t m_spritesheet_id{};
  int m_seed = 0;
  Vector2i m_tile_size{0, 0};
  std::map<uint32_t, SocietyBlueprint> m_societies;

  // Load information about tiles
  void m_load_tile_data();
  std::unordered_map<uint32_t, Action> m_load_actions();

  // Load information about items
  void m_load_item_data();
};
}  // namespace dl
