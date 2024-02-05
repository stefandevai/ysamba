#pragma once

#include "core/maths/vector.hpp"

namespace dl
{
struct CameraMovedEvent
{
  const Vector3& position;
  const Vector3i& position_in_tiles;
};

struct CameraZoomedEvent
{
  float zoom;
  const Vector2i& frustum;
};
}  // namespace dl
