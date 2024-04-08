#include "./nine_patch.hpp"

#include <glm/vec2.hpp>

namespace dl
{
NinePatch::NinePatch(const uint32_t id, const uint32_t resource_id) : id(id), resource_id(resource_id) {}

void NinePatch::generate_patches()
{
  assert(texture_atlas != nullptr && "TextureAtlas is null while trying to generate 9 patches");

  const auto texture = texture_atlas->texture.get();
  const auto& texture_size = texture_atlas->texture->size;
  const auto& data = texture_atlas->get_metadata<NinePatchData>(id);

  top = data.top;
  left = data.left;
  bottom = data.bottom;
  right = data.right;
  border = data.border;

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
}  // namespace dl
