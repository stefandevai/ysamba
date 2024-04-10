#include "./sprite_button.hpp"

#include <spdlog/spdlog.h>

#include "core/asset_manager.hpp"
#include "graphics/renderer/renderer.hpp"
#include "ui/components/container.hpp"
#include "ui/components/label.hpp"
#include "ui/components/mouse_region.hpp"

namespace dl::ui
{
SpriteButton::SpriteButton(UIContext& context, Params params)
    : UIComponent(context, "SpriteButton"),
      text_color(params.text_color),
      text_hover_color(params.text_hover_color),
      background_color(params.background_color),
      background_hover_color(params.background_hover_color)
{
  is_renderable = true;
  size = std::move(params.size);

  nine_patch.id = 4;
  nine_patch.resource_id = "ui"_hs;
  nine_patch.size.x = size.x;
  nine_patch.size.y = size.y;
  nine_patch.set_color(background_color);

  auto& on_mouse_over = params.on_mouse_over;
  auto& on_mouse_out = params.on_mouse_out;

  mouse_region = emplace<MouseRegion>(MouseRegion::Params{
      .on_left_click = std::move(params.on_left_click),
      .on_right_click = std::move(params.on_right_click),
      .on_mouse_over =
          [this, on_mouse_over]()
      {
        label->set_color(text_hover_color);
        nine_patch.set_color(background_hover_color);

        if (on_mouse_over)
        {
          on_mouse_over();
        }
      },
      .on_mouse_out =
          [this, on_mouse_out]()
      {
        label->set_color(text_color);
        nine_patch.set_color(background_color);

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

void SpriteButton::render()
{
  if (!is_active())
  {
    return;
  }

  if (nine_patch.color.opacity_factor != opacity)
  {
    nine_patch.color.opacity_factor = opacity;
  }

  m_context.renderer->ui_pass.batch.nine_patch(
      nine_patch, absolute_position.x, absolute_position.y, absolute_position.z);

  for (auto& child : children)
  {
    child->render();
  }
}
}  // namespace dl::ui
