#pragma once

#include <array>
#include <glm/glm.hpp>

#include "graphics/color.hpp"

namespace dl
{
class Texture;
class TextureAtlas;

struct TextureSlice
{
  uint32_t id{};
  uint32_t resource_id{};
  const Texture* texture = nullptr;
  Color color{0xFFFFFFFF};
  glm::vec2 size{};
  std::array<glm::vec2, 4> uv_coordinates{};

  void set_uv(const float top, const float left, const float bottom, const float right);
  void set_uv_with_size(const float top, const float left, const float width, const float height);
  void load_from_texture_atlas(TextureAtlas* texture_atlas);
};

template <typename Archive>
void serialize(Archive& archive, TextureSlice& slice)
{
  archive(slice.resource_id, slice.id);
}
}  // namespace dl
