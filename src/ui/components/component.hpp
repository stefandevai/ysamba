#pragma once

#include <vector>

#include "./types.hpp"
#include "core/maths/vector.hpp"

namespace dl
{
class Batch;
}

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

  virtual void update(const std::shared_ptr<Batch> batch);
  virtual void render(const std::shared_ptr<Batch> batch);
};

}  // namespace dl::ui
