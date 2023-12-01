#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "./components/component.hpp"

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

  void update();
  void render(Renderer& renderer);

 private:
  std::map<uint32_t, std::weak_ptr<UIComponent>> m_components;
  std::vector<glm::mat4> m_matrix_stack;

  static uint32_t m_identifier() noexcept
  {
    static uint32_t value = 0;
    return value++;
  }
};

}  // namespace dl::ui
