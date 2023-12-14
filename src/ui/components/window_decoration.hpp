#pragma once

#include "./component.hpp"
#include "graphics/quad.hpp"

namespace dl
{
struct Vector2i;
}

namespace dl::ui
{
class WindowDecoration : public UIComponent
{
 public:
  std::unique_ptr<Quad> quad = nullptr;

  WindowDecoration();
  WindowDecoration(const Vector2i& size, const uint32_t color);

  void render(Renderer& renderer, Batch& batch);

  void set_size(const Vector2i& size);
  void set_color(const uint32_t color);
};

}  // namespace dl::ui
