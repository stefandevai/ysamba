#include "./dummy_generator.hpp"

#include <vector>
#include "./tile_type.hpp"

// TEMP
#include <iostream>
// TEMP

namespace dl
{
  Tilemap DummyGenerator::generate(const int seed)
  {
    std::cout << "===================================\n";
    std::cout << "= STARTING DUMMY WORLD GENERATION =\n";
    std::cout << "===================================\n";
    std::cout << "SEED: " << seed << '\n';
    std::cout << "WIDTH: " << m_width << '\n';
    std::cout << "HEIGHT: " << m_height << "\n\n";

    std::vector<int> tiles(m_width * m_height);
    Tilemap tilemap{tiles, m_width, m_height};

    auto start = std::chrono::high_resolution_clock::now();

    for (int j = 0; j < m_height; ++j)
    {
      for (int i = 0; i < m_width; ++i)
      {
        tilemap.tiles[j*m_width + i] = TileType::Land;
      }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "[*] World generation finished! It took " << duration.count() << " milliseconds\n\n";

    return tilemap;
  }
}

