#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "./animation_manager.hpp"
#include "./components/component.hpp"
#include "./compositions/notification.hpp"
#include "./context.hpp"
#include "core/clock.hpp"

namespace dl
{
class AssetManager;
class Renderer;
}  // namespace dl

namespace dl::ui
{
class UIManager
{
 public:
  UIManager(AssetManager* asset_manager, Renderer* renderer);
  ~UIManager();

  // Remove copy/move constructors and assignment operators
  UIManager(UIManager const&) = delete;
  UIManager(UIManager const&&) = delete;
  void operator=(UIManager const&) = delete;
  void operator=(UIManager const&&) = delete;

  template <typename T, typename... Args>
  T* emplace(Args&&... args)
  {
    auto component = std::make_unique<T>(m_context, std::forward<Args>(args)...);
    m_components.push_back(std::move(component));
    return dynamic_cast<T*>(&(*m_components.back()));
  }

  template <typename T, typename... Args>
  void erase(T*& component)
  {
    auto it = std::find_if(m_components.begin(),
                           m_components.end(),
                           [component](std::unique_ptr<UIComponent>& c) { return c.get() == component; });
    if (it != m_components.end())
    {
      m_components.erase(it);
      component = nullptr;
    }
  }

  void update();
  void render();

  // Displays a notification with a message
  Notification* notify(const std::string& notification);

  // Hides all components without triggering animations (if any)
  void force_hide_all();

  // Utility to bring a component to the front of all the others
  void bring_to_front(UIComponent* component);

 private:
  AssetManager* m_asset_manager = nullptr;
  Renderer* m_renderer = nullptr;
  std::vector<std::unique_ptr<UIComponent>> m_components{};
  std::vector<std::unique_ptr<Notification>> m_notifications{};
  std::vector<glm::mat4> m_matrix_stack{};
  std::vector<UIComponent*> m_focused_stack{};
  AnimationManager m_animation_manager{};
  Clock m_clock{};

  UIContext m_context{
      this, m_asset_manager, m_renderer, &m_clock, &m_animation_manager.registry, &m_matrix_stack, &m_focused_stack};
};

}  // namespace dl::ui
