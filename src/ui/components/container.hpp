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

  Container();
  Container(const Vector2i& size, const std::string& color);

  void render(Renderer& renderer, const std::string& layer);

  void set_size(const Vector2i& size);
  void set_color(const std::string& color);
};

}  // namespace dl::ui
