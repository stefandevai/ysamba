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
  struct Params
  {
    Vector2i size{};
    uint32_t color = 0x00000000;
  };

  std::unique_ptr<Quad> quad = nullptr;

  Container(UIContext& context, Params params);

  void render();

  void set_size(const Vector2i& size);
  void set_color(const uint32_t color);
};

}  // namespace dl::ui
