#pragma once

#include <webgpu/wgpu.h>

#include <cstdint>

#include "graphics/renderer/pipelines/world.hpp"
#include "graphics/renderer/shader.hpp"

namespace dl
{
class AssetManager;
class Camera;
struct GameContext;
struct WGPUContext;

namespace v2
{
class Renderer
{
 public:
  WGPUContext& context;
  WorldPipeline world_pipeline{context};

  Renderer(GameContext& game_context);
  ~Renderer();

  void load();
  void resize();

  void clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a = 1.0f);
  void render(const Camera& camera);

 private:
  bool m_has_loaded = false;
  GameContext& m_game_context;
  WGPUTextureView depth_texture_view;
  WGPUTexture depth_texture;
  WGPUColor m_clear_color{0.0, 0.0, 0.0, 1.0};

  void m_load_depth_buffer(WGPUDevice device);
};

}  // namespace v2
}  // namespace dl
