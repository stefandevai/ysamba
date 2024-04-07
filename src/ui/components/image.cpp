#include "./image.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
Image::Image(UIContext& context, ImageType image) : UIComponent(context), image(std::move(image))
{
  is_renderable = true;
}

void Image::init()
{
  using namespace entt::literals;

  // nine_patch.id = 0;
  // nine_patch.resource_id = "ui"_hs;
  // nine_patch.size.x = size.x;
  // nine_patch.size.y = size.y;
}

void Image::render()
{
  if (!is_active())
  {
    return;
  }

  std::visit(ImageBatcher{m_context.renderer->ui_pass.batch, absolute_position, opacity}, image);

  // if (nine_patch.color.opacity_factor != opacity)
  // {
  //   nine_patch.color.opacity_factor = opacity;
  // }
  //
  // m_context.renderer->ui_pass.batch.nine_patch(
  //     nine_patch, absolute_position.x, absolute_position.y, absolute_position.z);

  for (auto& child : children)
  {
    child->render();
  }
}
}  // namespace dl::ui
