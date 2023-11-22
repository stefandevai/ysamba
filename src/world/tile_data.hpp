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
  std::vector<uint32_t> drop_ids;
  uint32_t after_removed;
};
}  // namespace dl
