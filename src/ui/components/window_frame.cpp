#include "./window_frame.hpp"

#include "core/asset_manager.hpp"
#include "core/maths/vector.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
WindowFrame::WindowFrame(UIContext& context) : UIComponent(context) {}

void WindowFrame::init()
{
  nine_patch.resource_id = "ui";
  nine_patch.texture = m_context.asset_manager->get<Texture>("ui");
  nine_patch.top = 0;
  nine_patch.left = 0;
  nine_patch.bottom = 32;
  nine_patch.right = 32;
  nine_patch.border = 12;
  nine_patch.size.x = size.x;
  nine_patch.size.y = size.y;
}

void WindowFrame::render(Batch& batch)
{
  if (state == State::Hidden)
  {
    return;
  }

  if (nine_patch.color.opacity_factor != opacity)
  {
    nine_patch.color.opacity_factor = opacity;
  }

  batch.nine_patch(nine_patch, absolute_position.x, absolute_position.y, absolute_position.z);

  for (auto& child : children)
  {
    child->render(batch);
  }
}
}  // namespace dl::ui
