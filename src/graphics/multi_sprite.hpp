#pragma once

#include <glm/vec2.hpp>

#include "./color.hpp"
#include "./renderable.hpp"
#include "./texture.hpp"
#include "./transform.hpp"

namespace dl
{
class MultiSprite : public Renderable
{
 public:
  std::string resource_id;
  std::shared_ptr<Texture> texture = nullptr;
  std::shared_ptr<Color> color = nullptr;
  std::shared_ptr<Transform> transform = nullptr;

  MultiSprite() {}
  MultiSprite(const std::string& resource_id,
              const std::vector<uint32_t>& frames,
              const uint32_t width,
              const uint32_t height)
      : resource_id(resource_id), m_frames(frames), m_width(width), m_height(height)
  {
  }

  const std::vector<uint32_t>& get_frames() const { return m_frames; };
  glm::vec2 get_size() const;
  // Get top-left, top-right, bottom-right and bottom-left uv coordinates
  std::array<glm::vec2, 4> get_texcoords(const uint32_t frame) const;

 private:
  std::vector<uint32_t> m_frames{};
  int m_width = 0;
  int m_height = 0;
};
}  // namespace dl
