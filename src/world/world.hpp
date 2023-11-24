#pragma once

#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <map>
#include <vector>

#include "../core/json.hpp"
#include "../core/lua_api.hpp"
#include "./item_data.hpp"
#include "./society_blueprint.hpp"
#include "./spatial_hash.hpp"
#include "./tile_data.hpp"
#include "./tilemap.hpp"

namespace dl
{
class Renderer;
struct TileTarget;

class World
{
 public:
  // Spatial hash for nearby entities search
  SpatialHash spatial_hash;

  // Constructor
  World();

  // Generate world
  void generate(const int width, const int height, const int seed);

  // Set tile by coordinates
  void set(const int tile_id, const int x, const int y, const int z);

  // Get tile data by coordinates
  const TileData& get(const int x, const int y, const int z) const;

  // Get size of a specific tilemap
  TilemapSize get_tilemap_size(const int z);

  // Get the seed used in this world generation
  int get_seed() const { return m_seed; };

  // Get the size of a loaded chunk
  size_t get_chunk_size() const { return m_chunk_size; };

  // Get the texture id for the tiles used in this world
  const std::string& get_texture_id() const { return m_texture_id; };

  // Get a specific society
  SocietyBlueprint get_society(const std::string& society_id) const { return m_societies.at(society_id); };

  // Get a nearby tile containing a flag
  TileTarget search_by_flag(const std::string& flag, const int x = 0, const int y = 0, const int z = 0) const;

  // Check if a specific tile is adjacent to a position
  bool adjacent(const int tile_id, const int x = 0, const int y = 0, const int z = 0) const;

  // Get information about a tile with id
  const TileData& get_tile_data(const uint32_t id) const;

  template <class Archive>
  void serialize(Archive& archive)
  {
    archive(m_depth_min, m_depth_max, m_tilemaps, m_seed, m_societies);
  }

 private:
  JSON m_json{"./data/world.json"};
  std::vector<Tilemap> m_tilemaps;
  std::unordered_map<uint32_t, TileData> m_tile_data;
  std::unordered_map<uint32_t, ItemData> m_item_data;
  size_t m_chunk_size = 0;
  std::string m_texture_id;
  int m_depth_min = 0;
  int m_depth_max = 1;
  int m_seed = 0;
  std::map<std::string, SocietyBlueprint> m_societies;

  // Load information about tiles
  void m_load_tile_data();

  // Load information about items
  void m_load_item_data();
};
}  // namespace dl
