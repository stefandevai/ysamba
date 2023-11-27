#pragma once

#include "./component.hpp"
#include "graphics/text.hpp"
#include "ui/style/text.hpp"

namespace dl::ui
{
class Label : public UIComponent
{
 public:
  Text text;
  TextStyle style;

  Label() : UIComponent(ComponentType::Text) {}

 private:
};

}  // namespace dl::ui