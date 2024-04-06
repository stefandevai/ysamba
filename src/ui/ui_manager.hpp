#pragma once

#include <entt/core/hashed_string.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

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
using namespace entt::literals;

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
    auto it = std::find_if(m_components.begin(), m_components.end(), [component](std::unique_ptr<UIComponent>& c) {
      return c.get() == component;
    });
    if (it != m_components.end())
    {
      m_components.erase(it);
      component = nullptr;
    }
  }

  void update();
  void render();
  Notification* notify(const std::string& notification);
  void force_hide_all();

 private:
  AssetManager* m_asset_manager = nullptr;
  Renderer* m_renderer = nullptr;
  std::vector<std::unique_ptr<UIComponent>> m_components{};
  std::vector<std::unique_ptr<Notification>> m_notifications{};
  std::vector<glm::mat4> m_matrix_stack{};
  AnimationManager m_animation_manager{};
  Clock m_clock{};

  UIContext m_context{m_asset_manager, m_renderer, &m_clock, &m_animation_manager.registry, &m_matrix_stack};
};

}  // namespace dl::ui
