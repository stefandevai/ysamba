#pragma once

#include <array>
#include <glm/glm.hpp>

#include "graphics/color.hpp"

namespace dl
{
class TextureAtlas;

struct SpriteFreeform
{
  uint32_t id{};
  uint32_t resource_id{};
  const TextureAtlas* texture = nullptr;
  Color color{0xFFFFFFFF};
  glm::vec2 size{};
  std::array<glm::vec2, 4> uv_coordinates{};
  std::string category{};

  void set_uv(const float top, const float left, const float bottom, const float right);
  void set_uv_with_size(const float top, const float left, const float width, const float height);
};

template <typename Archive>
void serialize(Archive& archive, SpriteFreeform& sprite)
{
  archive(sprite.resource_id, sprite.id, sprite.category);
}
}  // namespace dl
