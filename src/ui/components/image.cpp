#include "./image.hpp"

#include <spdlog/spdlog.h>

#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
Image::Image(UIContext& context, ImageType image) : UIComponent(context), image(std::move(image))
{
  is_renderable = true;

  std::visit(ImageLoader{*m_context.asset_manager}, image);
}

void Image::render()
{
  if (!is_active())
  {
    return;
  }

  std::visit(ImageBatcher{m_context.renderer->ui_pass.batch, absolute_position, opacity}, image);

  for (auto& child : children)
  {
    child->render();
  }
}
}  // namespace dl::ui
