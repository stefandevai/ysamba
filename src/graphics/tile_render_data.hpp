#pragma once

#include <array>
#include <glm/glm.hpp>

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
}  // namespace dl
