#include "./drop.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ai/actions/generic_item.hpp"
#include "ecs/components/action_drop.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/item_stack.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/utils.hpp"
#include "graphics/camera.hpp"
#include "ui/compositions/item_selection.hpp"
#include "ui/compositions/notification.hpp"
#include "ui/gameplay_modals.hpp"
#include "ui/ui_manager.hpp"
#include "world/item_factory.hpp"
#include "world/target.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_drop = [](entt::registry& registry, const entt::entity entity, const entt::entity job)
{
  auto& job_data = registry.get<JobData>(job);
  job_data.status = JobStatus::Finished;
  registry.remove<ActionDrop>(entity);
};

DropSystem::DropSystem(World& world, ui::UIManager& ui_manager, ui::GameplayModals& gameplay_modals)
    : m_world(world), m_gameplay_modals(gameplay_modals), m_ui_manager(ui_manager)
{
}

void DropSystem::update(entt::registry& registry, const Camera& camera)
{
  auto view = registry.view<ActionDrop>();

  for (const auto entity : view)
  {
    auto& action_drop = registry.get<ActionDrop>(entity);
    const auto& target = registry.get<Target>(action_drop.job);
    const entt::entity item = static_cast<entt::entity>(target.id);

    if (!registry.valid(item))
    {
      stop_drop(registry, entity, action_drop.job);
      continue;
    }

    bool has_item = utils::has_item(registry, entity, item);

    if (!has_item)
    {
      stop_drop(registry, entity, action_drop.job);
      continue;
    }

    utils::decrease_container_weight_and_volume_by_item(m_world, registry, item, 1);
    utils::remove_item_from_entity(registry, entity, item);

    auto& item_component = registry.get<Item>(item);

    // Add item to a stack on the ground if it is stackable and there is an item with the same id on the ground
    if (registry.all_of<ItemStack>(item))
    {
      // Check if an item with the same id is already in the position
      const auto items_on_ground = m_world.spatial_hash.get_all_by_component<Item>(
          target.position.x, target.position.y, target.position.z, registry);

      auto ground_item = std::find_if(items_on_ground.begin(),
                                      items_on_ground.end(),
                                      [&item_component, &registry](const auto& other)
                                      {
                                        const auto& other_item = registry.get<Item>(other);
                                        return other_item.id == item_component.id;
                                      });

      // Increase item stack
      if (ground_item != items_on_ground.end())
      {
        const auto& item_stack = registry.get<ItemStack>(item);
        auto& ground_item_stack = registry.get<ItemStack>(*ground_item);
        ground_item_stack.quantity += item_stack.quantity;
        registry.destroy(item);
        stop_drop(registry, entity, action_drop.job);
        continue;
      }
    }

    // Item is not stackable or there is no item with the same id on the ground.
    // Add item to the ground.
    registry.emplace<Position>(item,
                               static_cast<double>(target.position.x),
                               static_cast<double>(target.position.y),
                               static_cast<double>(target.position.z));
    registry.emplace<Sprite>(item,
                             Sprite{
                                 .resource_id = m_world.get_spritesheet_id(),
                                 .id = item_component.id,
                                 .category = "item",
                                 .layer_z = 2,
                             });

    stop_drop(registry, entity, action_drop.job);
  }

  if (m_ui_state == UIState::None)
  {
    m_process_input(registry);
  }
  else if (m_ui_state == UIState::SelectingTarget)
  {
    m_update_selecting_target(registry, camera);
  }
}

void DropSystem::m_process_input(entt::registry& registry)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("gameplay"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("drop_item"_hs))
  {
    const auto selected_entities = utils::get_selected_entities(registry);

    if (selected_entities.empty() || selected_entities.size() > 1)
    {
      return;
    }

    const auto on_select = [this](const ui::EntityPair& entities)
    {
      m_selected_entity = entities.first;
      m_target_item = entities.second;
      m_ui_state = UIState::SelectingTarget;
    };

    m_gameplay_modals.item_selection->set_title("Select Item");
    m_gameplay_modals.item_selection->set_on_select(std::move(on_select));
    m_gameplay_modals.item_selection->set_items_from_entity(registry, selected_entities[0]);
    m_gameplay_modals.item_selection->show();
  }
}

void DropSystem::m_update_selecting_target(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  if (!registry.valid(m_selected_entity) || !registry.valid(m_target_item))
  {
    m_dispose();
    return;
  }

  if (m_notification == nullptr)
  {
    m_gameplay_modals.item_selection->hide();
    m_notification = m_ui_manager.notify("Drop where?");
    m_input_manager.push_context("notification"_hs);
  }

  if (m_input_manager.poll_action("close"_hs))
  {
    m_dispose();
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    const auto mouse_tile = m_world.mouse_to_world(camera);

    action::generic_item::job({
        .registry = registry,
        .position = mouse_tile,
        .job_type = JobType::Drop,
        .entities = {m_selected_entity},
        .item = m_target_item,
    });

    m_dispose();
  }
}

void DropSystem::m_dispose()
{
  if (m_notification != nullptr)
  {
    m_notification->hide();
    m_notification = nullptr;
    m_input_manager.pop_context();
  }

  if (m_gameplay_modals.item_selection->is_visible())
  {
    m_gameplay_modals.item_selection->hide();
  }

  m_ui_state = UIState::None;
  m_target_item = entt::null;
  m_selected_entity = entt::null;
}
}  // namespace dl
