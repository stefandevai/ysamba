#include "./text_button.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/label.hpp"
#include "ui/components/mouse_region.hpp"

namespace dl::ui
{
TextButton::TextButton(UIContext& context, Params params)
    : UIComponent(context, "TextButton"), text_color(params.text_color), text_hover_color(params.text_hover_color)
{
  auto& on_mouse_over = params.on_mouse_over;
  auto& on_mouse_out = params.on_mouse_out;

  auto mouse_region = emplace<MouseRegion>(MouseRegionParams{
      .on_left_click = std::move(params.on_left_click),
      .on_right_click = std::move(params.on_right_click),
      .on_mouse_over =
          [this, on_mouse_over]()
      {
        label->set_color(text_hover_color);

        if (on_mouse_over)
        {
          on_mouse_over();
        }
      },
      .on_mouse_out =
          [this, on_mouse_out]()
      {
        label->set_color(text_color);

        if (on_mouse_out)
        {
          on_mouse_out();
        }
      },
  });

  label = mouse_region->emplace<Label>(LabelParams{
      .value = std::move(params.text),
      .color = text_color,
  });
}
}  // namespace dl::ui
