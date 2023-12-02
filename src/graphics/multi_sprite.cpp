#include "./multi_sprite.hpp"

namespace dl
{
glm::vec2 MultiSprite::get_size() const { return glm::vec2(m_width, m_height); }

// Get top-left, top-right, bottom-right and bottom-left uv coordinates
std::array<glm::vec2, 4> MultiSprite::get_texcoords(const uint32_t frame) const
{
  assert(texture != nullptr);
  return texture->get_frame_coords(frame);
}

}  // namespace dl
