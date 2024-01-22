#pragma once

#include <string>

#include "graphics/sprite.hpp"

namespace dl
{
struct Visibility
{
  std::unique_ptr<Sprite> sprite = nullptr;

  // Texture id
  std::string resource_id{};

  // Literal texture frame
  uint32_t frame{};

  // The id that relates to frame data for a texture
  uint32_t frame_id{};

  // The type that relates to frame data for a texture
  std::string frame_type{};

  // A z index to be applied only for rendering
  uint32_t layer_z{};

  // Angle to render the sprite at
  FrameAngle frame_angle = FrameAngle::Parallel;

  Visibility() = default;

  Visibility(const std::string& resource_id,
             const uint32_t frame = 0,
             const uint32_t layer_z = 0,
             const FrameAngle frame_angle = FrameAngle::Parallel)
      : resource_id(resource_id), frame(frame), layer_z(layer_z), frame_angle(frame_angle)
  {
    sprite = std::make_unique<Sprite>(resource_id, frame);
  };

  Visibility(const std::string& resource_id,
             const uint32_t frame_id,
             const std::string& frame_type,
             const uint32_t layer_z = 0)
      : resource_id(resource_id), frame_id(frame_id), frame_type(frame_type), layer_z(layer_z)
  {
    sprite = std::make_unique<Sprite>(resource_id, 0);
  };
};

template <typename Archive>
void serialize(Archive& archive, Visibility& visibility)
{
  archive(visibility.resource_id, visibility.frame, visibility.frame_id, visibility.frame_type, visibility.layer_z);
}
}  // namespace dl
