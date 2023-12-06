#pragma once

#include <map>
#include <string>

namespace dl
{
struct ItemData
{
  uint32_t id;
  std::string name;
  std::unordered_map<std::string, int> qualities{};
};
}  // namespace dl
