#pragma once

#include <webgpu/webgpu.h>

#include <cstdint>

namespace dl
{
class AssetManager;
class Camera;
struct GameContext;

namespace v2
{
class Renderer
{
 public:
  WGPUQueue queue;

  Renderer(GameContext& game_context);
  ~Renderer();

  void init();

  void clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a = 1.0f);
  void render(const Camera& camera);

 private:
  bool m_has_loaded = false;
  GameContext& m_game_context;
  WGPUTextureView depth_texture_view;
  WGPUTexture depth_texture;
};

}  // namespace v2
}  // namespace dl
