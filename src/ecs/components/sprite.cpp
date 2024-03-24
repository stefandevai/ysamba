#include "./sprite.hpp"

#include "graphics/renderer/spritesheet.hpp"

namespace dl
{
void Sprite::set_uv(const float top, const float left, const float bottom, const float right)
{
  uv_coordinates = std::array<glm::vec2, 4>{
      glm::vec2{left, top},
      glm::vec2{right, top},
      glm::vec2{right, bottom},
      glm::vec2{left, bottom},
  };
}

void Sprite::set_uv_with_size(const float top, const float left, const float width, const float height)
{
  assert(texture != nullptr && "Texture has not been initialized when setting uv with size");

  const auto& texture_size = texture->get_size();

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
}  // namespace dl
