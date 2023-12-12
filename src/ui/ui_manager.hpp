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

  template <typename T, typename... Args>
  T* emplace(Args&&... args)
  {
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    m_components.push_back(std::move(component));
    return dynamic_cast<T*>(&(*m_components.back()));
  }

  template <typename T, typename... Args>
  void erase(const T* component)
  {
    m_components.erase(std::find_if(m_components.begin(),
                                    m_components.end(),
                                    [component](std::unique_ptr<UIComponent>& c) { return c.get() == component; }));
  }

  void update();
  void render(Renderer& renderer);

 private:
  std::vector<std::unique_ptr<UIComponent>> m_components;
  std::vector<glm::mat4> m_matrix_stack;
  Batch m_batch{"default", 10};
  bool m_added_batch = false;
};

}  // namespace dl::ui
