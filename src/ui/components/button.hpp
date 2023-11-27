#pragma once

#include "./component.hpp"

namespace dl::ui
{
class Button : public UIComponent
{
 public:
  std::function<void()> on_click{};

  Button();
};

}  // namespace dl::ui
