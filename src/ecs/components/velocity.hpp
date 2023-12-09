#pragma once

namespace dl
{
struct Velocity
{
  double x;
  double y;
  double z;
};

template <typename Archive>
void serialize(Archive& archive, Velocity& velocity)
{
  archive(velocity.x, velocity.y, velocity.z);
}
}  // namespace dl
