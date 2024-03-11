#pragma once

namespace dl
{
struct Target
{
  Vector3 position;
  uint32_t id = 0;
  double distance_offset = 1.0;
};

template <typename Archive>
void serialize(Archive& archive, Target& target)
{
  archive(target.position, target.id, target.distance_offset);
}
}  // namespace dl
