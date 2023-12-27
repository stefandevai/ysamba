#pragma once

#include "core/maths/vector.hpp"

namespace dl
{
struct Chunk
{
  Vector3i position;
  bool active = false;

  Chunk() = default;
  Chunk(const Vector3i& position, const bool active)
  {
    this->position = position;
    this->active = active;
  }
};
}  // namespace dl
