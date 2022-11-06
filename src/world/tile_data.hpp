#pragma once

#include <string>

namespace dl
{
  struct TileData
  {
    int id;
    std::string name;
    int symbol;
    bool walkable;
    uint8_t r, g, b, a;
  };
}
