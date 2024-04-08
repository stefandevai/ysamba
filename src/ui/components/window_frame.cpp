#include "./window_frame.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "graphics/renderer/renderer.hpp"
#include "ui/components/image_button.hpp"

namespace dl::ui
{
WindowFrame::WindowFrame(UIContext& context) : UIComponent(context)
{
  is_renderable = true;
}

void WindowFrame::init()
{
  using namespace entt::literals;

  nine_patch.id = 0;
  nine_patch.resource_id = "ui"_hs;
  nine_patch.size.x = size.x;
  nine_patch.size.y = size.y;

  emplace<ImageButton>(ImageButtonParams{
      .image = TextureSlice{.id = 1, .resource_id = "ui"_hs},
      .on_left_click = [this]() { hide(); },
  });

  // close_button = emplace<Image>(TextureSlice{.id = 1, .resource_id = "ui"_hs});
  // close_button->x_alignment = XAlignement::Right;
  // close_button->size = Vector2i{17, 17};
  // close_button->margin = Vector2i{1, 1};
}

void WindowFrame::render()
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
