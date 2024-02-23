#pragma once

namespace dl
{
struct VertexData
{
  glm::vec3 position;
  glm::vec2 texture_coordinates;
  float texture_id;
  uint32_t color;
};

}  // namespace dl
