#pragma once

#include "./component.hpp"
#include "core/maths/vector.hpp"
#include "graphics/nine_patch.hpp"

namespace dl::ui
{
class Image;

class WindowFrame : public UIComponent
{
 public:
  struct Params
  {
    Vector2i size{};
    bool has_close_button = true;
  };

  NinePatch nine_patch{};
  Image* close_button = nullptr;

  WindowFrame(UIContext& context);
  WindowFrame(UIContext& context, Params params);

  void render();
  void hide();

  Vector2i get_safe_area_size();
  Vector2i get_position_offset();

 private:
  bool m_has_close_button = true;
};

}  // namespace dl::ui
