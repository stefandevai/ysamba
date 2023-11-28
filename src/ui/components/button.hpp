#pragma once

#include "./component.hpp"

namespace dl::ui
{
class Button : public UIComponent
{
 public:
  std::function<void()> on_click{};

  Button();

  void render(Batch& batch);
};

}  // namespace dl::ui
