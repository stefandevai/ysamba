#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace dl
{
enum class RuleType
{
  None,
  Identity,
  Autotile4Sides,
  Autotile8Sides,
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

struct AutoTile8SidesTransform
{
  int value;
};

struct AutoTile8SidesRule : public RuleBase
{
  int neighbor;
  std::array<AutoTile8SidesTransform, 47> output;
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
using Rule = std::variant<IdentityRule, AutoTile4SidesRule, AutoTile8SidesRule, UniformDistributionRule>;

struct TileValues
{
  int top_face;
  int top_face_decoration;
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

  // private:
  // static Rule m_create_rule(const nlohmann::json& rule, const RuleType type);
};
}  // namespace dl
