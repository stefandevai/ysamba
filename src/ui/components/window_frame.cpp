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

void WindowFrame::hide()
{
  // Hide topmost component instead of the current one
  UIComponent* current_component = this;

  while (current_component->parent != nullptr)
  {
    current_component = current_component->parent;
  }

  current_component->hide();
}
}  // namespace dl::ui
