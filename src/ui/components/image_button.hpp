#pragma once

#include <functional>

#include "./component.hpp"
#include "ui/components/image.hpp"

namespace dl::ui
{

struct ImageButtonParams
{
  ImageType image;
  std::function<void()> on_left_click{};
  std::function<void()> on_right_click{};
  std::function<void()> on_hover{};
};

class ImageButton : public UIComponent
{
 public:
  ImageButton(UIContext& context, ImageButtonParams params);
};

}  // namespace dl::ui
