#include "./dummy_generator.hpp"

#include <spdlog/spdlog.h>

#include <vector>

#include "./terrain_type.hpp"
#include "core/random.hpp"

namespace dl
{
Tilemap DummyGenerator::generate(const int seed)
{
  spdlog::info("===================================");
  spdlog::info("= STARTING DUMMY WORLD GENERATION =");
  spdlog::info("===================================\n");
  spdlog::info("SEED: {}", seed);
  spdlog::info("WIDTH: {}", m_width);
  spdlog::info("HEIGHT: {}\n", m_height);

  std::vector<int> tiles(m_width * m_height);
  Tilemap tilemap{tiles, m_width, m_height};

  for (int j = 0; j < m_height; ++j)
  {
    for (int i = 0; i < m_width; ++i)
    {
      const auto render_yuca = random::get_real();
      if (render_yuca < 0.1f)
      {
        tilemap.tiles[j * m_width + i] = TerrainType::Yuca;
        continue;
      }

      tilemap.tiles[j * m_width + i] = TerrainType::Land;
    }
  }

  return tilemap;
}
}  // namespace dl
