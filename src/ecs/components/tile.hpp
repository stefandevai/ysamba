#pragma once

#include <array>
#include <glm/glm.hpp>

namespace dl
{
class Spritesheet;
struct FrameData;

struct Tile
{
  const Spritesheet* texture = nullptr;
  const FrameData* frame_data = nullptr;
  const glm::vec2 size{};
};
}  // namespace dl
