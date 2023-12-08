#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "core/input_manager.hpp"
#include "core/maths/vector.hpp"

namespace dl
{
class Batch;
class Renderer;
}  // namespace dl

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
  Vector2i parent_size;
  XAlignement x_alignment = XAlignement::Left;
  YAlignement y_alignment = YAlignement::Top;
  Placement placement = Placement::Relative;

  std::vector<std::weak_ptr<UIComponent>> children;

  UIComponent() {}
  virtual ~UIComponent() {}

  virtual void update(std::vector<glm::mat4>& matrix_stack) { (void)matrix_stack; }
  virtual void update_component(std::vector<glm::mat4>& matrix_stack);
  virtual void render(Renderer& renderer, const std::string& layer);

 protected:
  static InputManager& m_input_manager;

  bool m_is_positioned();
};

}  // namespace dl::ui
