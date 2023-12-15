#include "./nine_patch.hpp"

namespace dl
{
NinePatch::NinePatch(const std::string& resource_id,
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

  const float texture_width = static_cast<float>(texture->get_width());
  const float texture_height = static_cast<float>(texture->get_height());

  const float uv_top = top / texture_height;
  const float uv_left = left / texture_width;
  const float uv_bottom = bottom / texture_height;
  const float uv_right = right / texture_width;

  const float uv_border_width = border / texture_width;
  const float uv_border_height = border / texture_height;

  // Top left patch
  border_patches[0].resource_id = resource_id;
  border_patches[0].texture = texture;
  border_patches[0].texture = texture;
  border_patches[0].color = color;
  border_patches[0].set_size(border, border);
  border_patches[0].set_litteral_uv(uv_top, uv_left, uv_top + uv_border_height, uv_left + uv_border_width);

  // Top center patch
  border_patches[1].resource_id = resource_id;
  border_patches[1].texture = texture;
  border_patches[1].color = color;
  border_patches[1].set_size(size.x - border * 2, border);
  border_patches[1].set_litteral_uv(
      uv_top, uv_left + uv_border_width, uv_top + uv_border_height, uv_right - uv_border_width);

  // Top right patch
  border_patches[2].resource_id = resource_id;
  border_patches[2].texture = texture;
  border_patches[2].color = color;
  border_patches[2].set_size(border, border);
  border_patches[2].set_litteral_uv(uv_top, uv_right - uv_border_width, uv_top + uv_border_height, uv_right);

  // Center right patch
  border_patches[3].resource_id = resource_id;
  border_patches[3].texture = texture;
  border_patches[3].color = color;
  border_patches[3].set_size(border, size.y - border * 2);
  border_patches[3].set_litteral_uv(
      uv_top + uv_border_height, uv_right - uv_border_width, uv_bottom - uv_border_height, uv_right);

  // Bottom right patch
  border_patches[4].resource_id = resource_id;
  border_patches[4].texture = texture;
  border_patches[4].color = color;
  border_patches[4].set_size(border, border);
  border_patches[4].set_litteral_uv(uv_bottom - uv_border_height, uv_right - uv_border_width, uv_bottom, uv_right);

  // Bottom center patch
  border_patches[5].resource_id = resource_id;
  border_patches[5].texture = texture;
  border_patches[5].color = color;
  border_patches[5].set_size(size.x - border * 2, border);
  border_patches[5].set_litteral_uv(
      uv_bottom - uv_border_height, uv_left + uv_border_width, uv_bottom, uv_right - uv_border_width);

  // Bottom left patch
  border_patches[6].resource_id = resource_id;
  border_patches[6].texture = texture;
  border_patches[6].color = color;
  border_patches[6].set_size(border, border);
  border_patches[6].set_litteral_uv(uv_bottom - uv_border_height, uv_left, uv_bottom, uv_left + uv_border_width);

  // Center left patch
  border_patches[7].resource_id = resource_id;
  border_patches[7].texture = texture;
  border_patches[7].color = color;
  border_patches[7].set_size(border, size.y - border * 2);
  border_patches[7].set_litteral_uv(
      uv_top + uv_border_height, uv_left, uv_bottom - uv_border_height, uv_left + uv_border_width);

  // Center patch
  center_patch.resource_id = resource_id;
  center_patch.texture = texture;
  center_patch.color = color;
  center_patch.set_size(size.x - border * 2, size.y - border * 2);
  center_patch.set_litteral_uv(
      uv_top + uv_border_height, uv_left + uv_border_width, uv_bottom - uv_border_height, uv_right - uv_border_width);

  dirty = false;
}

// Get top-left, top-right, bottom-right and bottom-left uv coordinates
std::array<glm::vec2, 4> NinePatch::get_texture_coordinates() const
{
  assert(texture != nullptr);

  const float texture_width = static_cast<float>(texture->get_width());
  const float texture_height = static_cast<float>(texture->get_height());

  const float top_uv = top / texture_height;
  const float left_uv = left / texture_width;
  const float bottom_uv = bottom / texture_height;
  const float right_uv = right / texture_width;

  return std::array<glm::vec2, 4>{
      glm::vec2{left_uv, top_uv},
      glm::vec2{right_uv, top_uv},
      glm::vec2{right_uv, bottom_uv},
      glm::vec2{left_uv, bottom_uv},
  };
}

}  // namespace dl
