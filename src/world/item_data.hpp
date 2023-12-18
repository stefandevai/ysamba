#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dl
{
struct ItemContainer
{
  double weight_capacity;
  double volume_capacity;
  std::vector<uint32_t> materials;
};

struct ItemData
{
  uint32_t id{};
  std::string name{};
  double weight{};
  std::string weight_string{};
  double volume{};
  std::string volume_string{};
  std::unordered_map<std::string, int> qualities{};
  std::unordered_set<std::string> flags{};
  std::vector<uint32_t> weared_on{};
  ItemContainer container;
};
}  // namespace dl
