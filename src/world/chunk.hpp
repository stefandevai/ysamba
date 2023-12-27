#pragma once

#include "./grid_3d.hpp"
#include "core/maths/vector.hpp"

namespace dl
{
struct Chunk
{
  Vector3i position;
  bool active = false;
  Grid3D tiles{};

  Chunk() = default;
  Chunk(const Vector3i& position, const bool active)
  {
    this->position = position;
    this->active = active;
  }
};
}  // namespace dl
