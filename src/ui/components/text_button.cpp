#include "./text_button.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/mouse_region.hpp"
#include "ui/components/label.hpp"

namespace dl::ui
{
TextButton::TextButton(UIContext& context, TextButtonParams params) : UIComponent(context, "TextButton")
{
  auto mouse_region = emplace<MouseRegion>(MouseRegionParams{
      .on_left_click = std::move(params.on_left_click),
      .on_right_click = std::move(params.on_right_click),
      .on_hover = std::move(params.on_hover),
  });

  label = mouse_region->emplace<Label>(LabelParams{
      .value = std::move(params.text),
  });
}
}  // namespace dl::ui
