#pragma once

#include <variant>

#include "./component.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/components/texture_slice.hpp"
#include "ecs/components/tile.hpp"

namespace dl
{
class AssetManager;
class Batch;
struct Vector3i;
}  // namespace dl

namespace dl::ui
{
using ImageType = std::variant<TextureSlice, Sprite>;

struct ImageLoader
{
  ImageLoader(UIComponent& component, AssetManager& asset_manager);

  void operator()(TextureSlice& slice);
  void operator()(Sprite& sprite);

 private:
  UIComponent& m_component;
  AssetManager& m_asset_manager;
};

struct ImageBatcher
{
  ImageBatcher(Batch& batch, const Vector3i& position, double opacity);

  void operator()(TextureSlice& slice);
  void operator()(Sprite& sprite);

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
