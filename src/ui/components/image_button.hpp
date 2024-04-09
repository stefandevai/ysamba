#pragma once

#include <functional>

#include "./component.hpp"
#include "ui/components/image.hpp"

namespace dl::ui
{

class ImageButton : public UIComponent
{
 public:
  struct Params
  {
    ImageType image;
    std::function<void()> on_left_click{};
    std::function<void()> on_right_click{};
    std::function<void()> on_mouse_over{};
    std::function<void()> on_mouse_out{};
  };

  ImageButton(UIContext& context, Params params);
};

}  // namespace dl::ui
