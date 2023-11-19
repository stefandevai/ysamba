#pragma once

#include <string>
#include <unordered_set>

namespace dl
{
struct TileData
{
  int id;
  std::string name;
  std::unordered_set<std::string> flags;
};
}  // namespace dl
