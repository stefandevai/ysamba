#pragma once

#include <entt/core/hashed_string.hpp>

#include "./component.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
using namespace entt::literals;

class Scrollable : public UIComponent
{
 public:
  Scrollable(UIContext& context);

  void update();
  void render();
  void reset_scroll();

 private:
  // Batch m_batch{"default"_hs, 99999};
  int m_scroll_y = 0;
};

}  // namespace dl::ui
