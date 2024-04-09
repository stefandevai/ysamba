#include "./image_button.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/mouse_region.hpp"

namespace dl::ui
{
ImageButton::ImageButton(UIContext& context, Params params) : UIComponent(context, "ImageButton")
{
  auto mouse_region = emplace<MouseRegion>(MouseRegion::Params{
      .on_left_click = std::move(params.on_left_click),
      .on_right_click = std::move(params.on_right_click),
      .on_mouse_over = std::move(params.on_mouse_over),
      .on_mouse_out = std::move(params.on_mouse_out),
  });

  mouse_region->emplace<Image>(std::move(params.image));
}
}  // namespace dl::ui
