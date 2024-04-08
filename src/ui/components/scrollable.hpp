#pragma once

#include <entt/core/hashed_string.hpp>

#include "./component.hpp"

namespace dl::ui
{
using namespace entt::literals;

class Scrollable : public UIComponent
{
 public:
  Scrollable(UIContext& context);

  void process_input();
  void update();
  void render();
  void reset_scroll();

 private:
  int m_scroll_y = 0;
};

}  // namespace dl::ui
