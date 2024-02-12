#include "./tile_rules.hpp"

#include <spdlog/spdlog.h>

#include "config.hpp"
#include "core/json.hpp"

namespace dl
{
std::unordered_map<int, Rule> TileRules::rules{};
Rule TileRules::identity = IdentityRule{0, "identity"};
bool TileRules::has_loaded = false;

Rule create_rule(const nlohmann::json& rule, const RuleType type)
{
  switch (type)
  {
  case RuleType::Autotile4Sides:
  {
    AutoTile4SidesRule rule_object;
    rule_object.input = rule["input"].get<int>();
    rule_object.label = rule["label"].get<std::string>();
    rule_object.neighbor = rule["neighbor"].get<int>();

    const auto& output = rule["output"];
    for (const auto& transform : output)
    {
      const auto value = transform["value"].get<int>();
      const auto index = transform["bitmask"].get<int>();

      rule_object.output[index].value = value;
    }

    return rule_object;
  }
  case RuleType::Autotile8Sides:
  {
    AutoTile8SidesRule rule_object;
    rule_object.input = rule["input"].get<int>();
    rule_object.label = rule["label"].get<std::string>();
    rule_object.neighbor = rule["neighbor"].get<int>();

    const auto& output = rule["output"];
    for (const auto& transform : output)
    {
      const auto value = transform["value"].get<int>();
      const auto bitmask = transform["bitmask"].get<int>();

      int index = -1;

      switch (bitmask)
      {
      case 25:
        index = 0;
        break;
      case 1:
        index = 1;
        break;
      case 35:
        index = 2;
        break;
      case 8:
        index = 3;
        break;
      case 0:
        index = 4;
        break;
      case 2:
        index = 5;
        break;
      case 140:
        index = 6;
        break;
      case 4:
        index = 7;
        break;
      case 70:
        index = 8;
        break;
      case 157:
        index = 9;
        break;
      case 5:
        index = 10;
        break;
      case 103:
        index = 11;
        break;
      case 59:
        index = 12;
        break;
      case 10:
        index = 13;
        break;
      case 206:
        index = 14;
        break;
      case 255:
        index = 15;
        break;
      case 249:
        index = 16;
        break;
      case 113:
        index = 17;
        break;
      case 177:
        index = 18;
        break;
      case 243:
        index = 19;
        break;
      case 216:
        index = 20;
        break;
      case 64:
        index = 21;
        break;
      case 128:
        index = 22;
        break;
      case 162:
        index = 23;
        break;
      case 184:
        index = 24;
        break;
      case 32:
        index = 25;
        break;
      case 16:
        index = 26;
        break;
      case 114:
        index = 27;
        break;
      case 252:
        index = 28;
        break;
      case 228:
        index = 29;
        break;
      case 212:
        index = 30;
        break;
      case 246:
        index = 31;
        break;
      case 248:
        index = 32;
        break;
      case 96:
        index = 33;
        break;
      case 144:
        index = 34;
        break;
      case 242:
        index = 35;
        break;
      case 241:
        index = 36;
        break;
      case 192:
        index = 37;
        break;
      case 48:
        index = 38;
        break;
      case 244:
        index = 39;
        break;
      case 240:
        index = 40;
        break;
      case 160:
        index = 41;
        break;
      case 80:
        index = 42;
        break;
      case 176:
        index = 43;
        break;
      case 208:
        index = 44;
        break;
      case 112:
        index = 45;
        break;
      case 224:
        index = 46;
        break;
      default:
        break;
      }

      if (index < 0)
      {
        spdlog::debug("Could not find index for bitmask: {}", bitmask);
        continue;
      }

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

void TileRules::load()
{
  if (has_loaded)
  {
    return;
  }

  JSON json{config::path::tile_rules};

  const auto& raw_rules = json.object["rules"];

  for (const auto& rule : raw_rules)
  {
    RuleType type = RuleType::None;

    if (rule["type"] == "autotile_4_sides")
    {
      type = RuleType::Autotile4Sides;
    }
    else if (rule["type"] == "autotile_8_sides")
    {
      type = RuleType::Autotile8Sides;
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
    const auto rule_object = create_rule(rule, type);

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

}  // namespace dl
