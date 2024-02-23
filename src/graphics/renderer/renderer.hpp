#pragma once

#include <webgpu/wgpu.h>

#include <cstdint>

#include "graphics/renderer/batch.hpp"
#include "graphics/renderer/shader.hpp"

namespace dl
{
class AssetManager;
class Camera;
struct GameContext;
struct WGPUContext;

class Renderer
{
 private:
  GameContext& m_game_context;

 public:
  WGPUContext& context;
  Batch batch{m_game_context};

  Renderer(GameContext& game_context);
  ~Renderer();

  void load();
  void resize();

  void clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a = 1.0f);
  void render(const Camera& camera);
  const Texture* get_texture(const uint32_t resource_id);

 private:
  bool m_has_loaded = false;
  WGPUTextureView depth_texture_view;
  WGPUTexture depth_texture;
  WGPUColor m_clear_color{0.0, 0.0, 0.0, 1.0};

  void m_load_depth_buffer(WGPUDevice device);
};

}  // namespace dl
