#pragma once

#include <cstdint>

namespace dl
{
class AssetManager;
class Camera;

namespace v2
{
class Renderer
{
 public:
  Renderer(AssetManager& asset_manager);

  void init();

  void clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a = 1.0f);
  void render(const Camera& camera);

 private:
  AssetManager& m_asset_manager;
};

}  // namespace v2
}  // namespace dl
