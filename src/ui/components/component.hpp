#pragma once

#include <entt/entity/registry.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>
#include <vector>

#include "core/input_manager.hpp"
#include "core/maths/vector.hpp"
#include "ui/animation.hpp"

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
  friend class UIManager;

  enum class State
  {
    Hidden,
    Visible,
    Animating,
  };

  bool dirty = true;
  State state = State::Visible;
  Vector3i position;
  Vector3i absolute_position;
  Vector2i size;
  XAlignement x_alignment = XAlignement::Left;
  YAlignement y_alignment = YAlignement::Top;
  Placement placement = Placement::Relative;
  entt::registry* animator = nullptr;
  entt::entity animations = entt::null;
  double opacity = 1.0;

  UIComponent* parent = nullptr;
  std::vector<std::unique_ptr<UIComponent>> children;

  UIComponent(const Vector2i& size = {0, 0});
  virtual ~UIComponent() {}

  virtual void init() {}
  virtual void update_geometry([[maybe_unused]] std::vector<glm::mat4>& matrix_stack) {}
  virtual void update([[maybe_unused]] const double delta) {}
  virtual void render(Renderer& renderer, Batch& batch);
  virtual void show();
  virtual void hide();

  template <typename T, typename... Args>
  T* emplace(Args&&... args)
  {
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    component->parent = this;

    if (animator != nullptr)
    {
      component->animator = animator;
    }

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

  template <typename T, typename... Args>
  void animate(Args&&... args)
  {
    assert(animator != nullptr && "Animation registry was not initialized when animating component");

    if (!animator->valid(animations))
    {
      animations = animator->create();
      animator->emplace<AnimationTarget>(animations, this);
    }

    animator->emplace<T>(animations, std::forward<Args>(args)...);

    if (state != State::Animating)
    {
      state = State::Animating;
    }
  }

 protected:
  static InputManager& m_input_manager;
  glm::mat4 m_transform_matrix{};
  bool m_has_initialized = false;

  void m_set_animator(entt::registry* animator);
  void m_update(const double delta, std::vector<glm::mat4>& matrix_stack);
  void m_update_geometry(std::vector<glm::mat4>& matrix_stack);
  bool m_is_positioned();
};

}  // namespace dl::ui
