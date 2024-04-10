#pragma once

#include "./component.hpp"
#include "core/maths/vector.hpp"
#include "graphics/nine_patch.hpp"
#include "graphics/quad.hpp"

namespace dl::ui
{
class NinePatchContainer : public UIComponent
{
 public:
  struct Params
  {
    uint32_t id = 0;
    Vector2i size{};
    uint32_t color = 0xFFFFFFFF;
  };

  NinePatch nine_patch{};

  NinePatchContainer(UIContext& context, Params params);

  void render();

  Vector2i get_safe_area_size();
  Vector2i get_position_offset();
  void set_size(const Vector2i size);
  void set_color(const uint32_t color);
};

}  // namespace dl::ui
