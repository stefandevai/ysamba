#pragma once

#include <webgpu/wgpu.h>

#include <cstdint>

#include "graphics/renderer/passes/main_render_pass.hpp"
#include "graphics/renderer/passes/ui_render_pass.hpp"
#include "graphics/renderer/shader.hpp"
#include "graphics/renderer/spritesheet.hpp"

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
  MainRenderPass main_pass{m_game_context};
  UIRenderPass ui_pass{m_game_context};

  Renderer(GameContext& game_context);
  ~Renderer();

  void load();
  void render(const Camera& camera);

  void resize();
  void clear_color(uint8_t r, uint8_t g, uint8_t b, float a = 1.0f);

 private:
  bool m_has_loaded = false;
  WGPUTexture depth_texture;
  WGPUTextureView depth_texture_view;

  // Descriptors used during rendering
  WGPUTextureViewDescriptor target_view_descriptor;
  WGPUCommandEncoderDescriptor command_encoder_descriptor;

  void m_load_depth_buffer(WGPUDevice device, WGPUTexture& texture, WGPUTextureView& texture_view);
};

}  // namespace dl
