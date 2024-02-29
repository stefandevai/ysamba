#pragma once

#include <array>
#include <glm/glm.hpp>

#include "core/maths/vector.hpp"
#include "graphics/color.hpp"
#include "graphics/frame_angle.hpp"

namespace dl
{
class Texture;
struct FrameData;

struct TileRenderData
{
  const Texture* texture = nullptr;
  const FrameData* frame_data = nullptr;
  const glm::vec2 size{};
  const std::array<glm::vec2, 4> uv_coordinates{};
};

struct SpriteRenderData
{
  uint32_t id{};
  uint32_t resource_id{};
  uint32_t layer_z{};
  const Texture* texture = nullptr;
  const FrameData* frame_data = nullptr;
  Color color{0xFFFFFFFF};
  glm::vec2 size{};
  glm::vec2 anchor{};
  std::array<glm::vec2, 4> uv_coordinates{};
  std::string category{};

  void set_uv(const float top, const float left, const float bottom, const float right)
  {
    uv_coordinates = std::array<glm::vec2, 4>{
        glm::vec2{left, top},
        glm::vec2{right, top},
        glm::vec2{right, bottom},
        glm::vec2{left, bottom},
    };
  }
};
}  // namespace dl
