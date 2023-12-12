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
  bool visible = true;
  bool dirty = true;

  std::vector<std::unique_ptr<UIComponent>> children;

  UIComponent(const Vector2i& size = {0, 0}) : size(size) {}
  virtual ~UIComponent() {}

  template <typename T, typename... Args>
  T* emplace(Args&&... args)
  {
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    children.push_back(std::move(component));
    return dynamic_cast<T*>(&(*children.back()));
  }

  template <typename T, typename... Args>
  void erase(const T* component)
  {
    children.erase(std::find_if(children.begin(), children.end(), [component](std::unique_ptr<UIComponent>& c) {
      return c.get() == component;
    }));
  }

  virtual void update([[maybe_unused]] std::vector<glm::mat4>& matrix_stack) {}
  virtual void update_component(std::vector<glm::mat4>& matrix_stack);
  virtual void render(Renderer& renderer, Batch& batch);
  virtual void show();
  virtual void hide();

 protected:
  static InputManager& m_input_manager;
  glm::mat4 m_transform_matrix{};

  bool m_is_positioned();
};

}  // namespace dl::ui
