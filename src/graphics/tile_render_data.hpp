#pragma once

#include <array>
#include <glm/glm.hpp>

namespace dl
{
namespace v2
{
class Texture;
struct FrameData;
}  // namespace v2

struct TileRenderData
{
  const v2::Texture* texture = nullptr;
  const v2::FrameData* frame_data = nullptr;
  const glm::vec2 size{};
  const std::array<glm::vec2, 4> uv_coordinates{};
};
}  // namespace dl
