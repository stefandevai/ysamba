#pragma once

#include <variant>

#include "./component.hpp"
#include "core/asset_manager.hpp"
#include "core/maths/vector.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/components/texture_slice.hpp"
#include "ecs/components/tile.hpp"
#include "graphics/renderer/batch.hpp"
#include "graphics/renderer/texture.hpp"

namespace dl::ui
{
using ImageType = std::variant<TextureSlice, Sprite>;

struct ImageLoader
{
  ImageLoader(AssetManager& asset_manager) : m_asset_manager(asset_manager) {}

  void operator()(TextureSlice& slice)
  {
    if (slice.texture == nullptr)
    {
      const auto texture_atlas = m_asset_manager.get<TextureAtlas>(slice.resource_id);
      slice.load_from_texture_atlas(texture_atlas);
    }
  }

  void operator()(Sprite& sprite)
  {
    if (sprite.spritesheet == nullptr)
    {
      sprite.spritesheet = m_asset_manager.get<Spritesheet>(sprite.resource_id);
      sprite.load_from_spritesheet();
    }
  }

 private:
  AssetManager& m_asset_manager;
};

struct ImageBatcher
{
  ImageBatcher(Batch& batch, const Vector3i& position, double opacity)
      : m_batch(batch), m_position(position), m_opacity(opacity)
  {
  }

  void operator()(TextureSlice& slice)
  {
    if (slice.color.opacity_factor != m_opacity)
    {
      slice.color.opacity_factor = m_opacity;
    }

    m_batch.texture_slice(slice, m_position.x, m_position.y, m_position.z);
  }

  void operator()(Sprite& sprite)
  {
    if (sprite.color.opacity_factor != m_opacity)
    {
      sprite.color.opacity_factor = m_opacity;
    }

    m_batch.sprite(sprite, m_position.x, m_position.y, m_position.z);
  }

 private:
  Batch& m_batch;
  const Vector3i& m_position;
  double m_opacity;
};

class Image : public UIComponent
{
 public:
  ImageType image;

  Image(UIContext& context, ImageType image);

  void render();
};

}  // namespace dl::ui
