#pragma once

#include <entt/entity/registry.hpp>

namespace dl::ui
{
class AnimationManager
{
 public:
  entt::registry registry;

  AnimationManager() = default;

  void update(const double delta);
};

}  // namespace dl::ui
