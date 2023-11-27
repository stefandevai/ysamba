#pragma once

#include <vector>

#include "./types.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
class UIComponent
{
 public:
  ComponentType type;
  Vector3i position;
  std::vector<std::shared_ptr<UIComponent>> children;

  UIComponent(ComponentType type) : type(type) {}
  virtual ~UIComponent() {}
};

}  // namespace dl::ui
