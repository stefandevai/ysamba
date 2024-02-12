#pragma once

#include <vector>

#include "world/society/sex.hpp"

namespace dl
{
struct Biology
{
  Biology() = default;
  Biology(const Sex sex, const int base_speed) : sex(sex), base_speed(base_speed), speed(base_speed) {}

  /* double age_in_days; */
  /* double height_in_cm; */
  /* double mass_in_g; */
  Sex sex{};

  // STR:10;END:10;QI:10;SOCIAL:INTRO;
  /* std::string genetics; */
  int base_speed{};
  int speed{};
  std::vector<uint32_t> body_parts{};
  double turn_threshold = 100.0;
};

template <typename Archive>
void serialize(Archive& archive, Biology& biology)
{
  archive(biology.sex, biology.base_speed, biology.speed, biology.turn_threshold);
}
}  // namespace dl
