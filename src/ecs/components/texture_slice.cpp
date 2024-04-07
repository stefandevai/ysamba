#include "./texture_slice.hpp"

#include "graphics/renderer/texture.hpp"
#include "graphics/renderer/texture_atlas.hpp"

namespace dl
{
void TextureSlice::set_uv(const float top, const float left, const float bottom, const float right)
{
  uv_coordinates = std::array<glm::vec2, 4>{
      glm::vec2{left, top},
      glm::vec2{right, top},
      glm::vec2{right, bottom},
      glm::vec2{left, bottom},
  };
}

void TextureSlice::set_uv_with_size(const float top, const float left, const float width, const float height)
{
  assert(texture != nullptr && "Texture has not been initialized when setting uv with size");

  const auto& texture_size = texture->size;

  const float uv_top = height / static_cast<float>(texture_size.y);
  const float uv_bottom = top / static_cast<float>(texture_size.y);
  const float uv_right = width / static_cast<float>(texture_size.x);

  uv_coordinates = std::array<glm::vec2, 4>{
      glm::vec2{left, uv_bottom - uv_top},
      glm::vec2{left + uv_right, uv_bottom - uv_top},
      glm::vec2{left + uv_right, uv_bottom},
      glm::vec2{left, uv_bottom},
  };

  size.x = width;
  size.y = height;
}

void TextureSlice::load_from_texture_atlas(TextureAtlas* texture_atlas)
{
  assert(texture_atlas != nullptr && "Texture atlas is null when loading TextureSlice");

  texture = texture_atlas->texture.get();
  const auto& texture_size = texture_atlas->texture->size;
  const auto& data = texture_atlas->get_metadata<TextureSliceData>(id);

  const auto uv_left = data.left / static_cast<float>(texture_size.x);
  const auto uv_top = data.top / static_cast<float>(texture_size.y);
  const auto uv_right = data.right / static_cast<float>(texture_size.x);
  const auto uv_bottom = data.bottom / static_cast<float>(texture_size.y);

  uv_coordinates = std::array<glm::vec2, 4>{
      glm::vec2{uv_left, uv_top},
      glm::vec2{uv_right, uv_top},
      glm::vec2{uv_right, uv_bottom},
      glm::vec2{uv_left, uv_bottom},
  };

  size.x = data.right - data.left;
  size.y = data.bottom - data.top;
}
}  // namespace dl
