#include "./image.hpp"

#include <spdlog/spdlog.h>

#include "core/asset_manager.hpp"
#include "core/maths/vector.hpp"
#include "graphics/renderer/batch.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{

ImageLoader::ImageLoader(UIComponent& component, AssetManager& asset_manager)
    : m_component(component), m_asset_manager(asset_manager)
{
}

void ImageLoader::operator()(TextureSlice& slice)
{
  if (slice.texture == nullptr)
  {
    const auto texture_atlas = m_asset_manager.get<TextureAtlas>(slice.resource_id);
    slice.load_from_texture_atlas(texture_atlas);
    m_component.size.x = slice.size.x;
    m_component.size.y = slice.size.y;
  }
}

void ImageLoader::operator()(Sprite& sprite)
{
  if (sprite.spritesheet == nullptr)
  {
    sprite.spritesheet = m_asset_manager.get<Spritesheet>(sprite.resource_id);
    sprite.load_from_spritesheet();
    m_component.size.x = sprite.size.x;
    m_component.size.y = sprite.size.y;
  }
}

ImageBatcher::ImageBatcher(Batch& batch, const Vector3i& position, double opacity)
    : m_batch(batch), m_position(position), m_opacity(opacity)
{
}

void ImageBatcher::operator()(TextureSlice& slice)
{
  if (slice.color.opacity_factor != m_opacity)
  {
    slice.color.opacity_factor = m_opacity;
  }

  m_batch.texture_slice(slice, m_position.x, m_position.y, m_position.z);
}

void ImageBatcher::operator()(Sprite& sprite)
{
  if (sprite.color.opacity_factor != m_opacity)
  {
    sprite.color.opacity_factor = m_opacity;
  }

  m_batch.sprite(sprite, m_position.x, m_position.y, m_position.z);
}

Image::Image(UIContext& context, ImageType image) : UIComponent(context), image(std::move(image))
{
  is_renderable = true;

  std::visit(ImageLoader{*this, *m_context.asset_manager}, image);
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
