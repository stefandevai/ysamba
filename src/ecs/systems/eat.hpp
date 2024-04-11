#pragma once

#include <entt/entity/registry.hpp>

#include "core/input_manager.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class GameplayModals;
class UIManager;
class Notification;
}  // namespace dl::ui

namespace dl
{
class World;
class Camera;
struct Vector2i;

class EatSystem
{
 public:
  EatSystem(World& world, entt::registry& registry, ui::GameplayModals& gameplay_modals);
  void update(entt::registry& registry, const Camera& camera);

 private:
  World& m_world;
  ui::GameplayModals& m_gameplay_modals;

  entt::entity m_selected_entity = entt::null;
  entt::entity m_target_item = entt::null;

  InputManager& m_input_manager = InputManager::get_instance();

  void m_update_selecting_target(entt::registry& registry, const Camera& camera);
  void m_process_input(entt::registry& registry);
  void m_dispose();
};
}  // namespace dl
