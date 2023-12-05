#pragma once

#include <glm/vec2.hpp>

#include "./color.hpp"
#include "./renderable.hpp"
#include "./texture.hpp"
#include "./transform.hpp"

namespace dl
{
class Sprite : public Renderable
{
 public:
  std::string resource_id;
  std::shared_ptr<Texture> texture = nullptr;
  std::shared_ptr<Color> color = nullptr;
  std::shared_ptr<Transform> transform = nullptr;

  Sprite() {}
  Sprite(const std::string& resource_id, const int frame = 0) : resource_id(resource_id), m_frame(frame) {}

  inline int get_frame() const { return m_frame; };
  glm::vec2 get_size() const;
  // Get top-left, top-right, bottom-right and bottom-left uv coordinates
  std::array<glm::vec2, 4> get_texture_coordinates() const;

  inline void set_frame(const int frame) { m_frame = frame; };
  void set_custom_uv(const float left, const float top, const float width, const float height);
  inline void increment_frame() { ++m_frame; }
  inline void decrement_frame() { --m_frame; }

 private:
  int m_frame = 0;
  bool m_has_custom_uv = false;
  glm::vec2 m_custom_uv{};
  float m_custom_width = 0.0f;
  float m_custom_height = 0.0f;
};
}  // namespace dl
