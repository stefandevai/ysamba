#include "./image_button.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/mouse_region.hpp"

namespace dl::ui
{
ImageButton::ImageButton(UIContext& context, ImageButtonParams params) : UIComponent(context, "ImageButton")
{
  auto mouse_region = emplace<MouseRegion>(MouseRegionParams{
      .on_left_click = std::move(params.on_left_click),
      .on_right_click = std::move(params.on_right_click),
      .on_hover = std::move(params.on_hover),
  });

  mouse_region->emplace<Image>(std::move(params.image));
}
}  // namespace dl::ui
