#pragma once

#include <functional>

#include "./component.hpp"

namespace dl::ui
{

struct MouseRegionParams
{
  std::function<void()> on_left_click;
  std::function<void()> on_right_click;
  std::function<void()> on_mouse_over{};
  std::function<void()> on_mouse_out{};
};

class MouseRegion : public UIComponent
{
 public:
  std::function<void()> on_left_click;
  std::function<void()> on_right_click;
  std::function<void()> on_mouse_over{};
  std::function<void()> on_mouse_out{};

  MouseRegion(UIContext& context, MouseRegionParams params);

  void after_init();
  void process_input();

 private:
  bool m_last_mouse_over = false;
};

}  // namespace dl::ui
