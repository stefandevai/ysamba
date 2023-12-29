#pragma once

#include <array>
#include <glm/glm.hpp>

namespace dl
{
class Texture;
struct FrameData;

struct TileRenderData
{
  Texture* texture;
  const FrameData* frame_data;
  const glm::vec2 size;
  const std::array<glm::vec2, 4> uv_coordinates;
};
}  // namespace dl
