#pragma once

#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <map>
#include <vector>

#include "../core/json.hpp"
#include "../core/lua_api.hpp"
#include "./society.hpp"
#include "./spatial_hash.hpp"
#include "./tile_data.hpp"
#include "./tilemap.hpp"

namespace dl
{
class Renderer;

class World
{
 public:
  SpatialHash spatial_hash;

  World();

  void generate(const int width, const int height, const int seed);
  const TileData get(const int x, const int y, const int z);
  TilemapSize get_tilemap_size(const int z);
  inline int get_seed() const { return m_seed; };
  inline size_t get_chunk_size() const { return m_chunk_size; };
  inline const std::string& get_texture_id() const { return m_texture_id; };
  inline Society get_society(const std::string& society_id) const { return m_societies.at(society_id); };

  template <class Archive>
  void serialize(Archive& archive)
  {
    archive(m_depth_min, m_depth_max, m_tilemaps, m_seed, m_societies);
  }

 private:
  JSON m_json{"./data/world.json"};
  /* std::vector<std::shared_ptr<Tilemap>> m_tilemaps; */
  std::vector<Tilemap> m_tilemaps;
  std::map<int, TileData> m_tile_data;
  size_t m_chunk_size = 0;
  std::string m_texture_id;
  int m_depth_min = 0;
  int m_depth_max = 1;
  int m_seed = 0;
  std::map<std::string, Society> m_societies;

  void m_load_tile_data();
};
}  // namespace dl
