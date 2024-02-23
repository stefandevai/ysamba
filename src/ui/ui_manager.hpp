#pragma once

#include <entt/core/hashed_string.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "./animation_manager.hpp"
#include "./components/component.hpp"
#include "./context.hpp"
#include "core/clock.hpp"
#include "graphics/batch.hpp"

namespace dl
{
// class Batch;
class AssetManager;

namespace v2
{
class Renderer;
}
}  // namespace dl

namespace dl::ui
{
using namespace entt::literals;

class UIManager
{
 public:
  UIManager(AssetManager* asset_manager, v2::Renderer* renderer);
  ~UIManager();

  template <typename T, typename... Args>
  T* emplace(Args&&... args)
  {
    auto component = std::make_unique<T>(m_context, std::forward<Args>(args)...);
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
  void render();
  void force_hide_all();

 private:
  AssetManager* m_asset_manager = nullptr;
  v2::Renderer* m_renderer = nullptr;
  std::vector<std::unique_ptr<UIComponent>> m_components;
  std::vector<glm::mat4> m_matrix_stack;
  // Batch m_batch{"default"_hs, 10};
  AnimationManager m_animation_manager{};
  Clock m_clock{};

  UIContext m_context{m_asset_manager, m_renderer, &m_clock, &m_animation_manager.registry, &m_matrix_stack};
};

}  // namespace dl::ui
