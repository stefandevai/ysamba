#pragma once

namespace dl
{
struct Target
{
  Vector3i position;
  uint32_t id = 0;
  uint32_t distance_offset = 1;
};

template <typename Archive>
void serialize(Archive& archive, Target& target)
{
  archive(target.position, target.id, target.distance_offset);
}
}  // namespace dl
