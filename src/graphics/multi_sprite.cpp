#include "./multi_sprite.hpp"

namespace dl
{
glm::vec2 MultiSprite::get_size() const { return glm::vec2(m_width, m_height); }

// Get top-left, top-right, bottom-right and bottom-left uv coordinates
std::array<glm::vec2, 4> MultiSprite::get_texture_coordinates() const
{
  assert(texture != nullptr);

  const int horizontal_frames = texture->get_horizontal_frames();
  const int vertical_frames = texture->get_vertical_frames();

  const float frame_width = 1.f / static_cast<float>(horizontal_frames);
  const float frame_height = 1.f / static_cast<float>(vertical_frames);

  const int max_frames = horizontal_frames * vertical_frames;
  const float frame_x = static_cast<float>(m_frame % horizontal_frames);
  const float frame_y = static_cast<float>((m_frame % max_frames) / horizontal_frames);

  const float top_left_x = frame_width * frame_x;
  const float top_left_y = frame_height * frame_y;
  const float bottom_right_x = frame_width * (frame_x + m_width);
  const float bottom_right_y = frame_height * (frame_y + m_height);

  return std::array<glm::vec2, 4>{
      glm::vec2{top_left_x, top_left_y},
      glm::vec2{bottom_right_x, top_left_y},
      glm::vec2{bottom_right_x, bottom_right_y},
      glm::vec2{top_left_x, bottom_right_y},
  };
}

}  // namespace dl
