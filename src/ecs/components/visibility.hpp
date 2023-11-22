#pragma once

#include <string>

#include "../../graphics/sprite.hpp"

namespace dl
{
struct Visibility
{
  std::shared_ptr<Sprite> sprite = nullptr;
  uint32_t frame_id{};
  std::string frame_type;

  Visibility(const std::string& resource_id, const uint32_t frame = 0)
  {
    sprite = std::make_shared<Sprite>(resource_id, frame);
  };

  Visibility(const std::string& resource_id, const uint32_t frame_id, const std::string& frame_type)
      : frame_id(frame_id), frame_type(frame_type)
  {
    sprite = std::make_shared<Sprite>(resource_id, 0);
  };
};
}  // namespace dl
