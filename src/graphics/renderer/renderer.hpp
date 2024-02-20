#pragma once

#include <webgpu/webgpu.h>

#include <cstdint>

#include "graphics/renderer/pipelines/world.hpp"
#include "graphics/renderer/shader.hpp"

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
  WorldPipeline world_pipeline{};
  Shader shader{};

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

  void m_load_depth_buffer();
};

}  // namespace v2
}  // namespace dl
