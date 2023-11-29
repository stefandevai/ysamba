#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "core/input_manager.hpp"
#include "core/maths/vector.hpp"

namespace dl
{
class Batch;
}

namespace dl::ui
{
enum class YAlignement
{
  Bottom,
  Center,
  Top,
};

enum class XAlignement
{
  Left,
  Center,
  Right,
};

enum class Placement
{
  Absolute,
  Relative,
};

class UIComponent
{
 public:
  Vector3i position;
  Vector3i absolute_position;
  Vector2i size;
  XAlignement x_alignment = XAlignement::Left;
  YAlignement y_alignment = YAlignement::Top;
  Placement placement = Placement::Relative;

  std::vector<std::shared_ptr<UIComponent>> children;

  UIComponent() {}
  virtual ~UIComponent() {}

  virtual void update(std::vector<glm::mat4>& matrix_stack) { (void)matrix_stack; }
  virtual void update_component(std::vector<glm::mat4>& matrix_stack);
  virtual void render(const std::shared_ptr<Batch> batch);

 protected:
  static const std::shared_ptr<InputManager> m_input_manager;

  bool m_is_positioned();
};

}  // namespace dl::ui
