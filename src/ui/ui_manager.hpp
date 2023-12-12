#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "./components/component.hpp"
#include "graphics/batch.hpp"

namespace dl
{
class Batch;
class Renderer;
}  // namespace dl

namespace dl::ui
{
class UIManager
{
 public:
  UIManager();
  ~UIManager();

  uint32_t add_component(const std::shared_ptr<UIComponent>& component);
  void remove_component(const uint32_t id);

  template <typename T, typename... Args>
  T& emplace(Args&&... args)
  {
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    m_components2.push_back(std::move(component));
    return dynamic_cast<T&>(*m_components2.back());
  }

  void erase(const UIComponent& component);

  void update();
  void render(Renderer& renderer);

 private:
  std::map<uint32_t, std::weak_ptr<UIComponent>> m_components;
  std::vector<std::unique_ptr<UIComponent>> m_components2;
  std::vector<glm::mat4> m_matrix_stack;
  Batch m_batch{"default", 10};
  bool m_added_batch = false;

  static uint32_t m_identifier() noexcept
  {
    static uint32_t value = 0;
    return value++;
  }
};

}  // namespace dl::ui
