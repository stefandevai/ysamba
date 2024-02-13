#pragma once

#include <glm/vec2.hpp>
#include <memory>

#include "./color.hpp"
#include "./texture.hpp"
#include "./transform.hpp"

namespace dl
{
class Sprite
{
 public:
  uint32_t resource_id{};
  const Texture* texture = nullptr;
  std::unique_ptr<Transform> transform = nullptr;
  Color color{0xFFFFFFFF};
  int frame = 0;
  FrameAngle frame_angle = FrameAngle::Parallel;

  Sprite() {}
  Sprite(const uint32_t resource_id, const int frame = 0, const FrameAngle frame_angle = FrameAngle::Parallel)
      : resource_id(resource_id), frame(frame), frame_angle(frame_angle)
  {
  }

  int get_frame() const { return frame; };
  const glm::vec2& get_size();

  // Get top-left, top-right, bottom-right and bottom-left uv coordinates
  const std::array<glm::vec2, 4>& get_texture_coordinates();

  void set_frame(const int frame);
  void increment_frame();
  void decrement_frame();
  void set_size(const float width, const float height);
  void set_custom_uv(const float top, const float left, const float width, const float height);
  void set_litteral_uv(const float top, const float left, const float bottom, const float right);

 private:
  bool m_dirty = true;
  bool m_has_custom_uv = false;
  glm::vec2 m_size;
  std::array<glm::vec2, 4> m_uv_coordinates{};
};
}  // namespace dl
