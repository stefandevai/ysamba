#include "./eat.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "ai/actions/generic_item.hpp"
#include "ecs/components/action_eat.hpp"
#include "ecs/components/carried_items.hpp"
#include "ecs/components/container.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/item_stack.hpp"
#include "ecs/components/item_target.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/sprite.hpp"
#include "ecs/components/weared_items.hpp"
#include "ecs/components/wielded_items.hpp"
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
const auto stop_eat = [](entt::registry& registry, const entt::entity entity, const entt::entity job)
{
  auto& job_data = registry.get<JobData>(job);
  job_data.status = JobStatus::Finished;
  registry.remove<ActionEat>(entity);
};

EatSystem::EatSystem(World& world, entt::registry& registry, ui::GameplayModals& gameplay_modals)
    : m_world(world), m_gameplay_modals(gameplay_modals)
{
  const auto on_select = [this, &registry](const ui::EntityPair& entities)
  {
    const auto job = registry.create();
    registry.emplace<JobData>(job, JobType::Eat);
    registry.emplace<ItemTarget>(job, entities.second);

    auto& agent = registry.get<SocietyAgent>(entities.first);
    agent.push_job(Job{0, job});

    m_gameplay_modals.item_selection->hide();
  };

  m_gameplay_modals.item_selection->set_on_select(std::move(on_select));
}

void EatSystem::update(entt::registry& registry, const Camera& camera)
{
  auto view = registry.view<ActionEat>();

  for (const auto entity : view)
  {
    auto& action_eat = registry.get<ActionEat>(entity);
    const auto& target = registry.get<ItemTarget>(action_eat.job);
    const auto item = target.item;

    if (!registry.valid(target.item))
    {
      stop_eat(registry, entity, action_eat.job);
      continue;
    }

    bool has_item = utils::has_item(registry, entity, item);

    if (!has_item)
    {
      stop_eat(registry, entity, action_eat.job);
      continue;
    }

    utils::decrease_container_weight_and_volume_by_item(m_world, registry, entity, item, 1);

    if (registry.all_of<ItemStack>(item))
    {
      auto& item_stack = registry.get<ItemStack>(item);
      --item_stack.quantity;

      // Item stack is empty, destroy item
      if (item_stack.quantity <= 0)
      {
        utils::remove_item_from_entity(registry, entity, item);
        registry.destroy(item);
      }
    }
    else
    {
      utils::remove_item_from_entity(registry, entity, item);
      registry.destroy(item);
    }

    stop_eat(registry, entity, action_eat.job);
  }

  m_process_input(registry);
}

void EatSystem::m_process_input(entt::registry& registry)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("gameplay"_hs))
  {
    return;
  }

  if (m_input_manager.poll_action("eat"_hs))
  {
    const auto selected_entities = utils::get_selected_entities(registry);

    if (selected_entities.empty() || selected_entities.size() > 1)
    {
      return;
    }

    m_gameplay_modals.item_selection->set_items_from_entity(registry, selected_entities[0]);
    m_gameplay_modals.item_selection->show();
  }
}

void EatSystem::m_dispose()
{
  if (m_gameplay_modals.item_selection->is_visible())
  {
    m_gameplay_modals.item_selection->hide();
  }

  m_target_item = entt::null;
  m_selected_entity = entt::null;
}
}  // namespace dl
