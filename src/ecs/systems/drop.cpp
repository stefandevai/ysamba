#include "./drop.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ecs/components/action_drop.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/container.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/visibility.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
#include "graphics/camera.hpp"
#include "ui/components/label.hpp"
#include "ui/compositions/item_selection.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_drop = [](entt::registry& registry, const entt::entity entity, const Job* job) {
  registry.remove<ActionDrop>(entity);
  job->status = JobStatus::Finished;
};

DropSystem::DropSystem(World& world, ui::UIManager& ui_manager) : m_world(world), m_ui_manager(ui_manager)
{
  const auto on_select = [this](const ui::EntityPair entities) {
    m_selected_entity = entities.first;
    m_target_item = entities.second;
    m_state = DropMenuState::SelectingTarget;
  };

  m_drop_menu = m_ui_manager.emplace<ui::ItemSelection>(std::move(on_select));

  m_select_target_label = m_ui_manager.emplace<ui::Label>("Select target");
  m_select_target_label->state = ui::UIComponent::State::Hidden;
  m_select_target_label->x_alignment = ui::XAlignement::Center;
  m_select_target_label->position.y = 30;
}

void DropSystem::update(entt::registry& registry, const Camera& camera)
{
  auto view = registry.view<ActionDrop>();

  for (const auto entity : view)
  {
    auto& action_drop = registry.get<ActionDrop>(entity);
    const auto& job = action_drop.job;
    const auto& target = job->target;
    const entt::entity item = static_cast<entt::entity>(target.id);

    if (!registry.valid(item))
    {
      stop_drop(registry, entity, job);
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
      registry.emplace<Position>(item, target.position.x, target.position.y, target.position.z);
      registry.emplace<Visibility>(item, m_world.get_texture_id(), item_component.id, "item", 1);
    }

    stop_drop(registry, entity, job);
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
          const auto& item_component = registry.get<Item>(left_hand);
          const auto& item = m_world.get_item_data(item_component.id);
          m_items.push_back({{entity, left_hand}, item.name});
        }
        if (registry.valid(right_hand) && right_hand != left_hand)
        {
          const auto& item_component = registry.get<Item>(right_hand);
          const auto& item = m_world.get_item_data(item_component.id);
          m_items.push_back({{entity, right_hand}, item.name});
        }
      }
      if (registry.all_of<WearedItems>(entity))
      {
        const auto& items = registry.get<WearedItems>(entity);

        for (auto item_entity : items.items)
        {
          const auto& item_component = registry.get<Item>(item_entity);
          const auto& item = m_world.get_item_data(item_component.id);
          m_items.push_back({{entity, item_entity}, item.name});
        }
      }
      if (registry.all_of<CarriedItems>(entity))
      {
        const auto& items = registry.get<CarriedItems>(entity);

        for (auto item_entity : items.items)
        {
          const auto& item_component = registry.get<Item>(item_entity);
          const auto& item = m_world.get_item_data(item_component.id);
          m_items.push_back({{entity, item_entity}, item.name});
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
  m_show_select_target_text();

  if (m_input_manager.poll_action("close_menu"_hs))
  {
    m_dispose();
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    const auto mouse_tile = m_world.mouse_to_world(camera);

    auto& agent = registry.get<SocietyAgent>(m_selected_entity);
    agent.jobs.push(Job{JobType::Walk, 2, Target{mouse_tile}});
    agent.jobs.push(Job{JobType::Drop, 2, Target{mouse_tile, static_cast<uint32_t>(m_target_item)}});

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

void DropSystem::m_show_select_target_text()
{
  assert(m_select_target_label != nullptr);

  if (m_select_target_label->state == ui::UIComponent::State::Hidden)
  {
    m_select_target_label->show();
  }
}

void DropSystem::m_close_drop_menu()
{
  if (m_drop_menu->state == ui::UIComponent::State::Visible)
  {
    m_drop_menu->hide();
  }
}

void DropSystem::m_close_select_target()
{
  if (m_select_target_label->state == ui::UIComponent::State::Visible)
  {
    m_select_target_label->hide();
  }
}

void DropSystem::m_dispose()
{
  m_close_drop_menu();
  m_close_select_target();

  m_state = DropMenuState::Closed;
  m_target_item = entt::null;
  m_selected_entity = entt::null;
  m_input_manager.pop_context();
}

void DropSystem::m_create_job(
    const JobType job_type, const uint32_t id, const Vector2i& position, entt::registry& registry, entt::entity entity)
{
  auto& agent = registry.get<SocietyAgent>(entity);
  agent.jobs.push(Job{JobType::Walk, 2, Target{Vector3i{position.x, position.y, 0}}});
  agent.jobs.push(Job{job_type, 2, Target{Vector3i{position.x, position.y, 0}, id}});
}

}  // namespace dl
