#pragma once

#include "./component.hpp"
#include "graphics/nine_patch.hpp"

namespace dl::ui
{
class Image;

class WindowFrame : public UIComponent
{
 public:
  NinePatch nine_patch{};
  Image* close_button = nullptr;

  WindowFrame(UIContext& context);

  void init();
  void render();
  void hide();
};

}  // namespace dl::ui
