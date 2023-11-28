#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "./types.hpp"
#include "core/input_manager.hpp"
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
  Vector3i absolute_position;
  std::vector<std::shared_ptr<UIComponent>> children;

  UIComponent(ComponentType type) : type(type) {}
  virtual ~UIComponent() {}

  virtual void update(std::vector<glm::mat4>& matrix_stack);
  virtual void render(const std::shared_ptr<Batch> batch);

 protected:
  static const std::shared_ptr<InputManager> m_input_manager;
};

}  // namespace dl::ui
