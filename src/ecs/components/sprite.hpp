#pragma once

#include <array>
#include <glm/glm.hpp>

#include "graphics/color.hpp"

namespace dl
{
class Spritesheet;
struct FrameData;

struct Sprite
{
  uint32_t id{};
  uint32_t resource_id{};
  uint32_t layer_z{};
  const Spritesheet* spritesheet = nullptr;
  const FrameData* frame_data = nullptr;
  Color color{0xFFFFFFFF};
  glm::vec2 size{};
  glm::vec2 anchor{};
  std::array<glm::vec2, 4> uv_coordinates{};
  std::string category{};

  void load_from_spritesheet();
};

template <typename Archive>
void serialize(Archive& archive, Sprite& sprite)
{
  archive(sprite.resource_id, sprite.id, sprite.layer_z, sprite.category);
}
}  // namespace dl
