#include "./dummy_generator.hpp"

#include <spdlog/spdlog.h>

#include <vector>

#include "./tile_type.hpp"

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

  auto start = std::chrono::high_resolution_clock::now();

  for (int j = 0; j < m_height; ++j)
  {
    for (int i = 0; i < m_width; ++i)
    {
      tilemap.tiles[j * m_width + i] = TileType::Land;
    }
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  spdlog::info("World generation finished! It took {} milliseconds", duration.count());

  return tilemap;
}
}  // namespace dl
