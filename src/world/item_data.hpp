#pragma once

#include <string>
#include <unordered_map>

namespace dl
{
struct ItemData
{
  uint32_t id;
  std::string name;
  std::unordered_map<std::string, int> qualities{};
};
}  // namespace dl
