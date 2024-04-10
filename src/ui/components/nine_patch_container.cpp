#include "./nine_patch_container.hpp"

#include "core/asset_manager.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
NinePatchContainer::NinePatchContainer(UIContext& context, Params params) : UIComponent(context, "NinePatchContainer")
{
  using namespace entt::literals;

  is_renderable = true;
  size = std::move(params.size);

  nine_patch.id = params.id;
  nine_patch.resource_id = "ui"_hs;
  nine_patch.size.x = size.x;
  nine_patch.size.y = size.y;
  nine_patch.set_color(params.color);
  nine_patch.texture_atlas = m_context.asset_manager->get<TextureAtlas>(nine_patch.resource_id);
  nine_patch.generate_patches();
}

void NinePatchContainer::render()
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

void NinePatchContainer::set_size(const Vector2i size)
{
  this->size = std::move(size);
  nine_patch.size.x = size.x;
  nine_patch.size.y = size.y;
  nine_patch.generate_patches();
}

void NinePatchContainer::set_color(const uint32_t color)
{
  nine_patch.set_color(color);
}
}  // namespace dl::ui
