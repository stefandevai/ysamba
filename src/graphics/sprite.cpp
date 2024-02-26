#include "./sprite.hpp"

namespace dl
{
Sprite::Sprite(const uint32_t resource_id, const uint32_t frame, const FrameAngle frame_angle)
    : resource_id(resource_id), frame(frame), frame_angle(frame_angle)
{
}

const glm::vec2& Sprite::get_size()
{
  if (m_dirty && !m_has_custom_uv)
  {
    assert(texture != nullptr && "Texture has not been initialized for sprite");

    m_uv_coordinates = texture->get_frame_coords(frame);
    const auto& frame_size = texture->get_frame_size();
    m_size = glm::vec2{frame_size.x, frame_size.y};
    m_dirty = false;
  }

  assert(m_size.x != 0.f && m_size.y != 0.f && "No width and height calculated");

  return m_size;
}

// Get top-left, top-right, bottom-right and bottom-left uv coordinates
const std::array<glm::vec2, 4>& Sprite::get_texture_coordinates()
{
  if (m_dirty && !m_has_custom_uv)
  {
    assert(texture != nullptr && "Texture has not been initialized for sprite");

    m_uv_coordinates = texture->get_frame_coords(frame);
    const auto& frame_size = texture->get_frame_size();
    m_size = glm::vec2{frame_size.x, frame_size.y};
    m_dirty = false;
  }

  return m_uv_coordinates;
}

void Sprite::set_frame(const uint32_t frame)
{
  this->frame = frame;
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

  const auto& texture_size = texture->get_size();

  const float uv_top = height / static_cast<float>(texture_size.y);
  const float uv_bottom = top / static_cast<float>(texture_size.y);
  const float uv_right = width / static_cast<float>(texture_size.x);

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
