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
  std::unique_ptr<Quad> quad = nullptr;

  Container();
  Container(const Vector2i& size, const uint32_t color);

  void render(Renderer& renderer, const std::string& layer);

  void set_size(const Vector2i& size);
  void set_color(const uint32_t color);
};

}  // namespace dl::ui
