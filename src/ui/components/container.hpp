#pragma once

#include "./component.hpp"
#include "graphics/quad.hpp"
#include "ui/style/container.hpp"

namespace dl::ui
{
class Container : public UIComponent
{
 public:
  std::shared_ptr<Quad> quad;
  ContainerStyle style;

  Container(const int width, const int height, const std::string& color);
};

}  // namespace dl::ui
