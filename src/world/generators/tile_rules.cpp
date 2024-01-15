#include "./tile_rules.hpp"

namespace dl
{
JSON TileRules::m_json = JSON{"./data/world/tile_rules.json"};
std::unordered_map<int, Rule> TileRules::rules{};
Rule TileRules::identity = IdentityRule{0, "identity"};
bool TileRules::has_loaded = false;

void TileRules::load()
{
  if (has_loaded)
  {
    return;
  }

  const auto& raw_rules = m_json.object["rules"];

  for (const auto& rule : raw_rules)
  {
    RuleType type = RuleType::None;

    if (rule["type"] == "autotile_4_sides")
    {
      type = RuleType::Autotile4Sides;
    }
    else if (rule["type"] == "uniform_distribution")
    {
      type = RuleType::UniformDistribution;
    }
    else
    {
      spdlog::warn("Invalid rule type: {}", rule["type"].get<std::string>());
      continue;
    }

    const auto input = rule["input"].get<int>();
    const auto rule_object = m_create_rule(rule, type);

    rules.insert({input, rule_object});
  }

  has_loaded = true;
}

const Rule& TileRules::get(const int input)
{
  const auto& rule = rules.find(input);

  if (rule == rules.end())
  {
    return identity;
  }

  return rule->second;
}

Rule TileRules::m_create_rule(const nlohmann::json& rule, const RuleType type)
{
  switch (type)
  {
  case RuleType::Autotile4Sides:
  {
    AutoTile4SidesRule rule_object;
    rule_object.input = rule["input"].get<int>();
    rule_object.label = rule["label"].get<std::string>();

    const auto& output = rule["output"];
    for (const auto& transform : output)
    {
      const auto value = transform["value"].get<int>();
      const auto index = transform["bitmask"].get<int>();

      rule_object.output[index].value = value;
    }

    return rule_object;
  }
  case RuleType::UniformDistribution:
  {
    UniformDistributionRule rule_object;
    rule_object.input = rule["input"].get<int>();
    rule_object.label = rule["label"].get<std::string>();

    const auto& output = rule["output"];
    for (const auto& transform : output)
    {
      UniformDistributionTransform transform_object;
      transform_object.value = transform["value"].get<int>();
      transform_object.probability = transform["probability"].get<double>();
      transform_object.placement =
          transform["placement"].get<std::string>() == "terrain" ? PlacementType::Terrain : PlacementType::Decoration;

      rule_object.output.push_back(transform_object);
    }

    return rule_object;
  }
  default:
  {
    return {};
  }
  }
}
}  // namespace dl
