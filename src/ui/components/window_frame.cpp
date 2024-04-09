#include "./window_frame.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/asset_manager.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/renderer/texture_atlas.hpp"
#include "ui/components/image_button.hpp"

namespace dl::ui
{
WindowFrame::WindowFrame(UIContext& context, Params params)
    : UIComponent(context, "WindowFrame"), m_has_close_button(params.has_close_button)
{
  using namespace entt::literals;

  is_renderable = true;
  size = std::move(params.size);

  nine_patch.id = 0;
  nine_patch.resource_id = "ui"_hs;
  nine_patch.size.x = size.x;
  nine_patch.size.y = size.y;
  nine_patch.texture_atlas = m_context.asset_manager->get<TextureAtlas>(nine_patch.resource_id);
  nine_patch.generate_patches();

  if (m_has_close_button)
  {
    emplace<ImageButton>(ImageButton::Params{
        .image = TextureSlice{.id = 1, .resource_id = "ui"_hs},
        .on_left_click = [this]() { hide(); },
    });
  }
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

Vector2i WindowFrame::get_safe_area_size()
{
  return Vector2i{
      static_cast<int>(size.x - nine_patch.border * 2),
      static_cast<int>(size.y - nine_patch.border * 2),
  };
}

Vector2i WindowFrame::get_position_offset()
{
  return Vector2i{
      static_cast<int>(nine_patch.border),
      static_cast<int>(nine_patch.border),
  };
}
}  // namespace dl::ui
