#pragma once

#include "./component.hpp"
#include "graphics/quad.hpp"
#include "ui/style/container.hpp"

namespace dl
{
struct Vector2i;
}

namespace dl::ui
{
class Container : public UIComponent
{
 public:
  std::shared_ptr<Quad> quad;

  Container(const Vector2i& size, const std::string& color);

  void render(const std::shared_ptr<Batch> batch);
};

}  // namespace dl::ui
