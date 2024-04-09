#include "./filled_button.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/container.hpp"
#include "ui/components/label.hpp"
#include "ui/components/mouse_region.hpp"

namespace dl::ui
{
FilledButton::FilledButton(UIContext& context, Params params)
    : UIComponent(context, "FilledButton"),
      text_color(params.text_color),
      text_hover_color(params.text_hover_color),
      background_color(params.background_color),
      background_hover_color(params.background_hover_color)
{
  size = std::move(params.size);
  auto& on_mouse_over = params.on_mouse_over;
  auto& on_mouse_out = params.on_mouse_out;

  mouse_region = emplace<MouseRegion>(MouseRegion::Params{
      .on_left_click = std::move(params.on_left_click),
      .on_right_click = std::move(params.on_right_click),
      .on_mouse_over =
          [this, on_mouse_over]()
      {
        label->set_color(text_hover_color);
        background->set_color(background_hover_color);

        if (on_mouse_over)
        {
          on_mouse_over();
        }
      },
      .on_mouse_out =
          [this, on_mouse_out]()
      {
        label->set_color(text_color);
        background->set_color(background_color);

        if (on_mouse_out)
        {
          on_mouse_out();
        }
      },
  });

  background = mouse_region->emplace<Container>(Container::Params{
      .size = size,
      .color = background_color,
  });

  label = background->emplace<Label>(Label::Params{
      .value = std::move(params.text),
      .color = text_color,
  });

  label->x_alignment = XAlignement::Center;
  label->y_alignment = YAlignement::Center;
}
}  // namespace dl::ui
