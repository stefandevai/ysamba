#pragma once

#include <cstdint>
#include <vector>

#include "world/society/sex.hpp"

namespace dl
{
struct Biology
{
  Biology() = default;
  Biology(const Sex sex, const uint32_t movement_cost) : sex(sex), movement_cost(movement_cost) {}

  /* double age_in_days; */
  /* double height_in_cm; */
  /* double mass_in_g; */
  Sex sex{};

  // STR:10;END:10;QI:10;SOCIAL:INTRO;
  /* std::string genetics; */
  int movement_cost{};
  std::vector<uint32_t> body_parts{};
  uint32_t energy = 0;
};

template <typename Archive>
void serialize(Archive& archive, Biology& biology)
{
  archive(biology.sex, biology.movement_cost, biology.energy, biology.body_parts);
}
}  // namespace dl
