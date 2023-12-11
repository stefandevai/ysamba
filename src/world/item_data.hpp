#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace dl
{
struct ItemData
{
  uint32_t id;
  std::string name;
  std::unordered_map<std::string, int> qualities{};
  std::unordered_set<std::string> flags;
};
}  // namespace dl
