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

  void init();

  void clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a = 1.0f);
  void render(const Camera& camera);

 private:
  GameContext& m_game_context;
};

}  // namespace v2
}  // namespace dl
