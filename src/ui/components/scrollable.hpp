#pragma once

#include "./component.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
class Scrollable : public UIComponent
{
 public:
  Scrollable(UIContext& context);

  void update();
  void render(Batch& batch);
  void reset_scroll();

 private:
  Batch m_batch{"default", 99999};
  int m_scroll_y = 0;
};

}  // namespace dl::ui
