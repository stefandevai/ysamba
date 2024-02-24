#pragma once

#include <webgpu/wgpu.h>

#include <cstdint>

#include "graphics/renderer/render_passes/main_pass.hpp"
#include "graphics/renderer/render_passes/ui_pass.hpp"
#include "graphics/renderer/shader.hpp"

namespace dl
{
class Camera;
struct GameContext;
struct WGPUContext;
class AssetManager;

class Renderer
{
 private:
  GameContext& m_game_context;

 public:
  WGPUContext& context;
  AssetManager& asset_manager;
  MainPass main_pass{m_game_context};
  UIPass ui_pass{m_game_context};

  Renderer(GameContext& game_context);
  ~Renderer();

  void load();
  void render(const Camera& camera);

  void resize();
  void clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a = 1.0f);
  const Texture* get_texture(const uint32_t resource_id);

 private:
  bool m_has_loaded = false;
  WGPUTextureView depth_texture_view;
  WGPUTexture depth_texture;

  // Descriptors used during rendering
  WGPUTextureViewDescriptor target_view_descriptor;
  WGPUCommandEncoderDescriptor command_encoder_descriptor;

  void m_load_depth_buffer(WGPUDevice device);
};

}  // namespace dl
