#include "./nine_patch.hpp"

#include <glm/vec2.hpp>

namespace dl
{
NinePatch::NinePatch(const uint32_t resource_id,
                     const uint32_t top,
                     const uint32_t left,
                     const uint32_t bottom,
                     const uint32_t right,
                     const uint32_t border)
    : resource_id(resource_id), top(top), left(left), bottom(bottom), right(right), border(border)
{
}

void NinePatch::generate_patches()
{
  assert(texture != nullptr && "Texture is null while trying to generate 9 patches");

  const auto& texture_size = texture->get_size();

  const float uv_top = top / static_cast<float>(texture_size.y);
  const float uv_left = left / static_cast<float>(texture_size.x);
  const float uv_bottom = bottom / static_cast<float>(texture_size.y);
  const float uv_right = right / static_cast<float>(texture_size.x);

  const float uv_border_width = border / static_cast<float>(texture_size.x);
  const float uv_border_height = border / static_cast<float>(texture_size.y);

  // Top left patch
  border_patches[0] = {
      .resource_id = resource_id,
      .texture = texture,
      .color = color,
      .size = glm::vec2(border, border),
  };
  border_patches[0].set_uv(uv_top, uv_left, uv_top + uv_border_height, uv_left + uv_border_width);

  // Top center patch
  border_patches[1] = {
      .resource_id = resource_id,
      .texture = texture,
      .color = color,
      .size = glm::vec2(size.x - border * 2, border),
  };
  border_patches[1].set_uv(uv_top, uv_left + uv_border_width, uv_top + uv_border_height, uv_right - uv_border_width);

  // Top right patch
  border_patches[2] = {
      .resource_id = resource_id,
      .texture = texture,
      .color = color,
      .size = glm::vec2(border, border),
  };
  border_patches[2].set_uv(uv_top, uv_right - uv_border_width, uv_top + uv_border_height, uv_right);

  // Center right patch
  border_patches[3] = {
      .resource_id = resource_id,
      .texture = texture,
      .color = color,
      .size = glm::vec2(border, size.y - border * 2),
  };
  border_patches[3].set_uv(
      uv_top + uv_border_height, uv_right - uv_border_width, uv_bottom - uv_border_height, uv_right);

  // Bottom right patch
  border_patches[4] = {
      .resource_id = resource_id,
      .texture = texture,
      .color = color,
      .size = glm::vec2(border, border),
  };
  border_patches[4].set_uv(uv_bottom - uv_border_height, uv_right - uv_border_width, uv_bottom, uv_right);

  // Bottom center patch
  border_patches[5] = {
      .resource_id = resource_id,
      .texture = texture,
      .color = color,
      .size = glm::vec2(size.x - border * 2, border),
  };
  border_patches[5].set_uv(
      uv_bottom - uv_border_height, uv_left + uv_border_width, uv_bottom, uv_right - uv_border_width);

  // Bottom left patch
  border_patches[6] = {
      .resource_id = resource_id,
      .texture = texture,
      .color = color,
      .size = glm::vec2(border, border),
  };
  border_patches[6].set_uv(uv_bottom - uv_border_height, uv_left, uv_bottom, uv_left + uv_border_width);

  // Center left patch
  border_patches[7] = {
      .resource_id = resource_id,
      .texture = texture,
      .color = color,
      .size = glm::vec2(border, size.y - border * 2),
  };
  border_patches[7].set_uv(uv_top + uv_border_height, uv_left, uv_bottom - uv_border_height, uv_left + uv_border_width);

  // Center patch
  center_patch = {
      .resource_id = resource_id,
      .texture = texture,
      .color = color,
      .size = glm::vec2(size.x - border * 2, size.y - border * 2),
  };
  center_patch.set_uv(
      uv_top + uv_border_height, uv_left + uv_border_width, uv_bottom - uv_border_height, uv_right - uv_border_width);

  dirty = false;
}

// Get top-left, top-right, bottom-right and bottom-left uv coordinates
std::array<glm::vec2, 4> NinePatch::get_texture_coordinates() const
{
  assert(texture != nullptr);

  const auto& texture_size = texture->get_size();

  const float top_uv = top / static_cast<float>(texture_size.y);
  const float left_uv = left / static_cast<float>(texture_size.x);
  const float bottom_uv = bottom / static_cast<float>(texture_size.y);
  const float right_uv = right / static_cast<float>(texture_size.x);

  return std::array<glm::vec2, 4>{
      glm::vec2{left_uv, top_uv},
      glm::vec2{right_uv, top_uv},
      glm::vec2{right_uv, bottom_uv},
      glm::vec2{left_uv, bottom_uv},
  };
}

}  // namespace dl
