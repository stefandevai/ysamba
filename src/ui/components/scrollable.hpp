#pragma once

#include "./component.hpp"

namespace dl::ui
{
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
