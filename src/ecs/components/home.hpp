#pragma once

#include "core/maths/vector.hpp"

namespace dl
{
struct Home
{
  uint32_t id = 0;
  Vector3i size{};
};

template <typename Archive>
void serialize(Archive& archive, Home& home)
{
  archive(home.id, home.size);
}
}  // namespace dl
