#pragma once

#include <functional>

#include "./component.hpp"

namespace dl::ui
{

struct MouseRegionParams
{
  std::function<void()> on_left_click;
  std::function<void()> on_right_click;
  std::function<void()> on_hover;
};

class MouseRegion : public UIComponent
{
 public:
  std::function<void()> on_left_click;
  std::function<void()> on_right_click;
  std::function<void()> on_hover;

  MouseRegion(UIContext& context, MouseRegionParams params);

  void after_init();
  void process_input();
};

}  // namespace dl::ui
