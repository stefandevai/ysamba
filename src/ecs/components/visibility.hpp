#pragma once

#include <string>

#include "graphics/sprite.hpp"

namespace dl
{
struct Visibility
{
  std::unique_ptr<Sprite> sprite = nullptr;

  std::string resource_id{};

  // The id that relates to frame data for a texture
  uint32_t frame_id{};

  // The type that relates to frame data for a texture
  std::string frame_type{};

  // A z index to be applied only for rendering
  uint32_t layer_z{};

  Visibility() = default;

  Visibility(const std::string& resource_id, const uint32_t frame = 0, const uint32_t layer_z = 0)
      : resource_id(resource_id), layer_z(layer_z)
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
}  // namespace dl
