#include "./drop.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ai/actions/generic_item.hpp"
#include "ecs/components/action_drop.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/container.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/item_stack.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "graphics/camera.hpp"
#include "ui/compositions/item_selection.hpp"
#include "ui/compositions/notification.hpp"
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

DropSystem::DropSystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  const auto on_select = [this](const ui::EntityPair entities)
  {
    m_selected_entity = entities.first;
    m_target_item = entities.second;
    m_state = DropMenuState::SelectingTarget;
  };

  m_drop_menu = m_ui_manager.emplace<ui::ItemSelection>(std::move(on_select));
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

    auto& wielded = registry.get<WieldedItems>(entity);
    auto& weared = registry.get<WearedItems>(entity);
    auto& carried = registry.get<CarriedItems>(entity);

    bool removed = false;

    if (wielded.left_hand == item)
    {
      wielded.left_hand = entt::null;
      removed = true;
    }
    else if (wielded.right_hand == item)
    {
      wielded.right_hand = entt::null;
      removed = true;
    }

    if (!removed)
    {
      for (auto it = carried.items.begin(); it != carried.items.end(); ++it)
      {
        if (*it != item)
        {
          continue;
        }

        auto& item_component = registry.get<Item>(*it);
        const auto& item_data = m_world.get_item_data(item_component.id);

        assert(registry.valid(item_component.container) && "Encountered carried item without container");

        auto& container = registry.get<Container>(item_component.container);
        container.weight_occupied -= item_data.weight;
        container.volume_occupied -= item_data.volume;

        item_component.container = entt::null;
        container.items.erase(std::find(container.items.begin(), container.items.end(), *it));
        carried.items.erase(it);
        removed = true;
        break;
      }
    }

    if (!removed)
    {
      for (auto it = weared.items.begin(); it != weared.items.end(); ++it)
      {
        if (*it == item)
        {
          weared.items.erase(it);
          removed = true;
          break;
        }
      }
    }

    // Only update item components if we were able to find the item in the agent inventory
    if (removed)
    {
      auto& item_component = registry.get<Item>(item);

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

      // Add item to the ground
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
    }

    stop_drop(registry, entity, action_drop.job);
  }

  if (m_state == DropMenuState::Open)
  {
    m_update_drop_menu();
  }
  else if (m_state == DropMenuState::Closed)
  {
    m_update_closed_menu(registry);
  }
  else if (m_state == DropMenuState::SelectingTarget)
  {
    m_update_selecting_target(registry, camera);
  }
}

void DropSystem::m_update_drop_menu()
{
  using namespace entt::literals;

  m_open_drop_menu();

  if (m_input_manager.poll_action("close_menu"_hs))
  {
    m_dispose();
  }
}

void DropSystem::m_update_closed_menu(entt::registry& registry)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("gameplay"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("drop_item"_hs))
  {
    m_items.clear();
    std::vector<entt::entity> selected_entities{};
    auto view = registry.view<Selectable>();

    for (const auto entity : view)
    {
      const auto& selectable = registry.get<Selectable>(entity);

      if (!selectable.selected)
      {
        continue;
      }

      selected_entities.push_back(entity);
    }

    if (selected_entities.empty())
    {
      return;
    }

    for (const auto entity : selected_entities)
    {
      if (registry.all_of<WieldedItems>(entity))
      {
        const auto& items = registry.get<WieldedItems>(entity);
        const auto left_hand = items.left_hand;
        const auto right_hand = items.right_hand;

        if (registry.valid(left_hand))
        {
          m_items.push_back({{entity, left_hand}, item_factory::get_item_label(m_world, registry, left_hand)});
        }
        if (registry.valid(right_hand) && right_hand != left_hand)
        {
          m_items.push_back({{entity, right_hand}, item_factory::get_item_label(m_world, registry, right_hand)});
        }
      }
      if (registry.all_of<WearedItems>(entity))
      {
        const auto& items = registry.get<WearedItems>(entity);

        for (auto item_entity : items.items)
        {
          m_items.push_back({{entity, item_entity}, item_factory::get_item_label(m_world, registry, item_entity)});
        }
      }
      if (registry.all_of<CarriedItems>(entity))
      {
        const auto& items = registry.get<CarriedItems>(entity);

        for (auto item_entity : items.items)
        {
          m_items.push_back({{entity, item_entity}, item_factory::get_item_label(m_world, registry, item_entity)});
        }
      }
    }

    m_drop_menu->set_items(m_items);
    m_state = DropMenuState::Open;
    m_input_manager.push_context("item_selection_menu"_hs);
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

  m_close_drop_menu();

  if (m_notification == nullptr)
  {
    m_notification = m_ui_manager.notify("Drop where?");
  }

  if (m_input_manager.poll_action("close_menu"_hs))
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

void DropSystem::m_open_drop_menu()
{
  assert(m_drop_menu != nullptr);

  if (m_drop_menu->state == ui::UIComponent::State::Hidden)
  {
    m_drop_menu->show();
  }
}

void DropSystem::m_close_drop_menu()
{
  if (m_drop_menu->state == ui::UIComponent::State::Visible)
  {
    m_drop_menu->hide();
  }
}

void DropSystem::m_dispose()
{
  if (m_notification != nullptr)
  {
    m_notification->hide();
    m_notification = nullptr;
  }

  m_close_drop_menu();

  m_state = DropMenuState::Closed;
  m_target_item = entt::null;
  m_selected_entity = entt::null;
  m_input_manager.pop_context();
}
}  // namespace dl
