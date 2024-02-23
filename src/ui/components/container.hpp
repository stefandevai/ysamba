#pragma once

#include "./component.hpp"
#include "graphics/quad.hpp"

namespace dl
{
struct Vector2i;
}

namespace dl::ui
{
class Container : public UIComponent
{
 public:
  std::unique_ptr<Quad> quad = nullptr;

  Container(UIContext& context);
  Container(UIContext& context, const Vector2i& size, const uint32_t color);

  void render();

  void set_size(const Vector2i& size);
  void set_color(const uint32_t color);
};

}  // namespace dl::ui
