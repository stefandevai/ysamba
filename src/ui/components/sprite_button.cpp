#include "./sprite_button.hpp"

#include <spdlog/spdlog.h>

#include "core/asset_manager.hpp"
#include "graphics/renderer/renderer.hpp"
#include "ui/components/container.hpp"
#include "ui/components/label.hpp"
#include "ui/components/mouse_region.hpp"
#include "ui/components/nine_patch_container.hpp"

namespace dl::ui
{
SpriteButton::SpriteButton(UIContext& context, Params params)
    : UIComponent(context, "SpriteButton"),
      text_color(params.text_color),
      text_hover_color(params.text_hover_color),
      background_color(params.background_color),
      background_hover_color(params.background_hover_color)
{
  size = std::move(params.size);

  container = emplace<NinePatchContainer>(NinePatchContainer::Params{
      .id = 4,
      .size = size,
      .color = background_color,
  });

  auto& on_mouse_over = params.on_mouse_over;
  auto& on_mouse_out = params.on_mouse_out;

  mouse_region = container->emplace<MouseRegion>(MouseRegion::Params{
      .on_left_click = std::move(params.on_left_click),
      .on_right_click = std::move(params.on_right_click),
      .on_mouse_over =
          [this, on_mouse_over]()
      {
        container->set_color(background_hover_color);
        label->set_color(text_hover_color);

        if (on_mouse_over)
        {
          on_mouse_over();
        }
      },
      .on_mouse_out =
          [this, on_mouse_out]()
      {
        container->set_color(background_color);
        label->set_color(text_color);

        if (on_mouse_out)
        {
          on_mouse_out();
        }
      },
  });

  label = mouse_region->emplace<Label>(Label::Params{
      .value = std::move(params.text),
      .color = text_color,
  });
  mouse_region->size = size;

  label->x_alignment = XAlignement::Center;
  label->y_alignment = YAlignement::Center;
}
}  // namespace dl::ui
