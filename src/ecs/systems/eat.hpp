#pragma once

#include <entt/fwd.hpp>

#include "core/input_manager.hpp"

namespace dl::ui
{
class GameplayModals;
}  // namespace dl::ui

namespace dl
{
class World;

class EatSystem
{
 public:
  EatSystem(World& world, ui::GameplayModals& gameplay_modals);
  void update(entt::registry& registry);

 private:
  World& m_world;
  ui::GameplayModals& m_gameplay_modals;
  InputManager& m_input_manager = InputManager::get_instance();

  void m_process_input(entt::registry& registry);
};
}  // namespace dl
