#pragma once

#include "./component.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
class Scrollable : public UIComponent
{
 public:
  Scrollable();

  void update(const double delta);
  void render(Renderer& renderer, Batch& batch);

 private:
  Batch m_batch{"default", 99999};
  bool m_added_batch = false;
  int m_scroll_y = 0;
};

}  // namespace dl::ui
