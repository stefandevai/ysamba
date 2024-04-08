#include "./sprite.hpp"

#include "graphics/frame_data.hpp"
#include "graphics/renderer/spritesheet.hpp"

namespace dl
{
void Sprite::load_from_spritesheet()
{
  assert(spritesheet != nullptr && "Spritesheet is null");

  frame_data = &spritesheet->id_to_frame(id, category);
  assert(frame_data != nullptr && "Frame data is null");

  const auto& frame_size = spritesheet->get_frame_size();
  uv_coordinates = spritesheet->get_uv_coordinates(frame_data->faces[frame_data->default_face]);
  size = glm::vec2{frame_size.x * frame_data->width, frame_size.y * frame_data->height};
  anchor = glm::vec2{frame_size.x * frame_data->anchor_x, frame_size.y * frame_data->anchor_y};
}
}  // namespace dl
