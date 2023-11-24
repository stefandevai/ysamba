#pragma once

#include "../../world/society/sex.hpp"

namespace dl
{
struct Biology
{
  Biology(const Sex sex, const int base_speed) : sex(sex), base_speed(base_speed), speed(base_speed) {}

  /* double age_in_days; */
  /* double height_in_cm; */
  /* double mass_in_g; */
  Sex sex;

  // STR:10;END:10;QI:10;SOCIAL:INTRO;
  /* std::string genetics; */
  int base_speed;
  int speed;
  double turn_threshold = 100.0;
};
}  // namespace dl
