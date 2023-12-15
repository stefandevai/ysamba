#pragma once

#include "./component.hpp"
#include "graphics/nine_patch.hpp"

namespace dl
{
struct Vector2i;
}

namespace dl::ui
{
class WindowFrame : public UIComponent
{
 public:
  NinePatch nine_patch{};

  WindowFrame(UIContext& context);

  void init();
  void render(Batch& batch);
};

}  // namespace dl::ui
