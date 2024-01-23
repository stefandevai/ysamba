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
  int input;
  std::string label;
};

struct AutoTile4SidesTransform
{
  int value;
};

struct AutoTile4SidesRule : public RuleBase
{
  int neighbor;
  std::array<AutoTile4SidesTransform, 16> output;
};

struct UniformDistributionTransform
{
  int value;
  double probability;
  PlacementType placement;
};

struct UniformDistributionRule : public RuleBase
{
  std::vector<UniformDistributionTransform> output;
};

using IdentityRule = RuleBase;
using Rule = std::variant<IdentityRule, AutoTile4SidesRule, UniformDistributionRule>;

struct TileValues
{
  int terrain;
  int decoration;
};

class TileRules
{
 public:
  static std::unordered_map<int, Rule> rules;
  static Rule identity;
  static bool has_loaded;

  TileRules() = delete;

  static void load();
  static const Rule& get(const int input);

 private:
  static JSON m_json;

  static Rule m_create_rule(const nlohmann::json& rule, const RuleType type);
};
}  // namespace dl
