#include "./sprite.hpp"

namespace dl
{
const glm::vec2& Sprite::get_size()
{
  if (m_dirty && !m_has_custom_uv)
  {
    m_uv_coordinates = texture->get_frame_coords(frame);
    m_size.x = texture->get_frame_width();
    m_size.y = texture->get_frame_height();
    m_dirty = false;
  }

  assert(m_size.x != 0.f && m_size.y != 0.f && "No width and height calculated");

  return m_size;
}

// Get top-left, top-right, bottom-right and bottom-left uv coordinates
const std::array<glm::vec2, 4>& Sprite::get_texture_coordinates()
{
  assert(texture != nullptr && "Texture has not been initialized for sprite");

  if (m_dirty && !m_has_custom_uv)
  {
    m_uv_coordinates = texture->get_frame_coords(frame);
    m_size.x = texture->get_frame_width();
    m_size.y = texture->get_frame_height();
    m_dirty = false;
  }

  return m_uv_coordinates;
}

void Sprite::set_frame(const int frame)
{
  this->frame = frame;
  m_dirty = true;
}

void Sprite::increment_frame()
{
  ++frame;
  m_dirty = true;
}

void Sprite::decrement_frame()
{
  --frame;
  m_dirty = true;
}

void Sprite::set_size(const float width, const float height)
{
  m_size.x = width;
  m_size.y = height;
}

void Sprite::set_custom_uv(const float top, const float left, const float width, const float height)
{
  assert(texture != nullptr && "Texture has not been initialized for setting custom uv");

  const auto texture_width = texture->get_width();
  const auto texture_height = texture->get_height();

  const auto uv_top = height / texture_height;
  const auto uv_bottom = top / texture_height;
  const auto uv_right = width / texture_width;

  m_uv_coordinates = std::array<glm::vec2, 4>{
      glm::vec2{left, uv_bottom - uv_top},
      glm::vec2{left + uv_right, uv_bottom - uv_top},
      glm::vec2{left + uv_right, uv_bottom},
      glm::vec2{left, uv_bottom},
  };

  m_size.x = width;
  m_size.y = height;
  m_has_custom_uv = true;
  m_dirty = false;
}

void Sprite::set_litteral_uv(const float top, const float left, const float bottom, const float right)
{
  m_uv_coordinates = std::array<glm::vec2, 4>{
      glm::vec2{left, top},
      glm::vec2{right, top},
      glm::vec2{right, bottom},
      glm::vec2{left, bottom},
  };

  m_dirty = false;
}

}  // namespace dl
