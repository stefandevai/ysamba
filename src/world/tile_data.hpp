#pragma once

#include <unordered_set>
#include <string>

namespace dl
{
  struct TileData
  {
    int id;
    std::string name;
    std::unordered_set<std::string> flags;
  };
}
