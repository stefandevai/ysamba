#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "world/society/effect.hpp"

namespace dl
{
enum class MatterState
{
  None,
  Solid,
  Liquid,
  Gas,
  Plasma,
};

struct ItemContainer
{
  double weight_capacity;
  double volume_capacity;
  std::vector<MatterState> matter_states;
};

struct ItemData
{
  uint32_t id{};
  MatterState matter_state{};
  double weight{};
  double volume{};
  std::string name{};
  std::string weight_string{};
  std::string volume_string{};
  std::vector<uint32_t> weared_on{};
  std::unordered_map<std::string, int> qualities{};
  std::unordered_set<std::string> flags{};
  ItemContainer container;
  std::vector<Effect> on_consume_effects{};
};
}  // namespace dl
