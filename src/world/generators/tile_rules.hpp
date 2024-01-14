#pragma once

#include <array>
#include <string>
#include <variant>
#include <vector>

#include "core/json.hpp"

namespace dl
{
enum class RuleType
{
  None,
  Identity,
  Autotile4Sides,
  UniformDistribution,
};

enum class PlacementType
{
  Terrain,
  Decoration,
};

struct RuleBase
{
  uint32_t input;
  std::string label;
  RuleType type;
};

struct AutoTile4SidesTransform
{
  uint32_t value;
};

struct AutoTile4SidesRule : public RuleBase
{
  std::array<AutoTile4SidesTransform, 16> output;
};

struct UniformDistributionTransform
{
  uint32_t value;
  double probability;
  PlacementType placement;
};

struct UniformDistributionRule : public RuleBase
{
  std::vector<UniformDistributionTransform> output;
};

using Rule = std::variant<RuleBase, AutoTile4SidesRule, UniformDistributionRule>;

class TileRules
{
 public:
  static std::unordered_map<uint32_t, Rule> rules;
  static Rule identity;
  static bool has_loaded;

  TileRules() = delete;

  static void load();
  static const Rule& get(const uint32_t input);

 private:
  static JSON m_json;

  static Rule m_create_rule(const nlohmann::json& rule, const RuleType type);
};
}  // namespace dl
