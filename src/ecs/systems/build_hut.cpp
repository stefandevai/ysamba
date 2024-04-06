#include "./build_hut.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "ai/actions/create_hut.hpp"
#include "ai/actions/walk.hpp"
#include "core/events/action.hpp"
#include "core/events/emitter.hpp"
#include "core/json.hpp"
#include "core/random.hpp"
#include "ecs/components/action_build_hut.hpp"
#include "ecs/components/action_place_hut_exterior.hpp"
#include "ecs/components/action_walk.hpp"
#include "ecs/components/biology.hpp"
#include "ecs/components/home.hpp"
#include "ecs/components/home_floor.hpp"
#include "ecs/components/job_data.hpp"
#include "ecs/components/job_data_build_hut.hpp"
#include "ecs/components/job_progress.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "graphics/camera.hpp"
#include "ui/compositions/notification.hpp"
#include "ui/ui_manager.hpp"
#include "world/target.hpp"
#include "world/world.hpp"

namespace dl
{
const auto stop_build_hut = [](entt::registry& registry, const entt::entity entity, const entt::entity job)
{
  if (registry.valid(job))
  {
    auto& job_data = registry.get<JobData>(job);
    auto& progress = registry.get<JobProgress>(job_data.progress_entity);
    progress.agents.erase(std::remove(progress.agents.begin(), progress.agents.end(), entity));
    job_data.status = JobStatus::Finished;
  }
  registry.remove<ActionBuildHut>(entity);
};

const auto stop_place_exterior = [](entt::registry& registry, const entt::entity entity, const entt::entity job)
{
  if (registry.valid(job))
  {
    auto& job_data = registry.get<JobData>(job);
    job_data.status = JobStatus::Finished;
  }
  registry.remove<ActionPlaceHutExterior>(entity);
};

BuildHutSystem::BuildHutSystem(World& world, EventEmitter& event_emitter, ui::UIManager& ui_manager)
    : m_world(world), m_ui_manager(ui_manager)
{
  event_emitter.on<SelectHutTargetEvent>(
      [this](const SelectHutTargetEvent& event, EventEmitter& emitter)
      {
        (void)emitter;
        (void)event;
        m_state = State::SelectHutTarget;
      });
}

void BuildHutSystem::update(entt::registry& registry)
{
  using namespace entt::literals;

  auto view = registry.view<ActionBuildHut, Biology, const Position>();
  for (const auto entity : view)
  {
    auto& action_build_hut = registry.get<ActionBuildHut>(entity);

    if (!registry.valid(action_build_hut.job))
    {
      stop_build_hut(registry, entity, action_build_hut.job);
      continue;
    }

    auto& job_data = registry.get<JobData>(action_build_hut.job);

    if (!registry.valid(job_data.progress_entity))
    {
      stop_build_hut(registry, entity, action_build_hut.job);
      continue;
    }

    if (job_data.status != JobStatus::InProgress)
    {
      continue;
    }

    assert(registry.all_of<JobProgress>(job_data.progress_entity));
    auto& job_progress = registry.get<JobProgress>(job_data.progress_entity);

    if (job_progress.progress < job_progress.total_cost)
    {
      // Consumate work
      auto& biology = registry.get<Biology>(entity);

      if (biology.energy <= biology.work_cost)
      {
        continue;
      }

      biology.energy -= biology.work_cost;
      job_progress.progress += 100;
      m_random_walk(registry, entity, action_build_hut.job);
      continue;
    }

    // Job is done
    // Walk outside of hut bounds
    // Agent is already walking
    if (registry.all_of<ActionWalk>(entity))
    {
      continue;
    }

    // NOTE: Taking target as a reference causes a weird bug where the top part
    // is placed placed with an offset such as target is modified in some way.
    // This is a workaround.
    const auto target = registry.get<Position>(job_data.progress_entity);
    auto& job_data_build_hut = registry.get<JobDataBuildHut>(action_build_hut.job);
    const uint32_t hut_size = job_data_build_hut.hut_size;
    const auto& position = registry.get<Position>(entity);

    if (m_is_within_hut_bounds(position, target, hut_size))
    {
      const uint32_t distance_to_bottom = (target.y + hut_size) - position.y;
      const uint32_t distance_to_top = position.y - (target.y - 1);
      const uint32_t distance_to_left = position.x - (target.x - 1);
      const uint32_t distance_to_right = (target.x + hut_size) - position.x;

      const uint32_t min = std::min({distance_to_bottom, distance_to_top, distance_to_left, distance_to_right});

      Vector3i new_position{position.x, position.y, position.z};

      if (min == distance_to_bottom)
      {
        new_position.y = target.y + hut_size;
      }
      else if (min == distance_to_top)
      {
        new_position.y = target.y - 1;
      }
      else if (min == distance_to_left)
      {
        new_position.x = target.x - 1;
      }
      else if (min == distance_to_right)
      {
        new_position.x = target.x + hut_size;
      }

      action::walk::job({
          .registry = registry,
          .agent_entity = entity,
          .position = new_position,
          .priority = 0,
      });
      continue;
    }

    // Agent is not within hut bounds.
    // Remove inactive agents or agents with invalid jobs
    std::erase_if(job_progress.agents,
                  [&registry](const auto entity)
                  {
                    if (!registry.valid(entity))
                    {
                      return true;
                    }

                    if (!registry.all_of<ActionBuildHut>(entity))
                    {
                      return true;
                    }

                    const auto& agent = registry.get<SocietyAgent>(entity);

                    if (agent.jobs.empty())
                    {
                      return true;
                    }

                    const auto& current_job = agent.jobs.front();

                    if (!registry.valid(current_job.entity))
                    {
                      return true;
                    }

                    return false;
                  });

    // If there are still agents within the hut bounds, remove the build hut action, freeing them for other jobs
    if (job_progress.agents.size() > 1)
    {
      stop_build_hut(registry, entity, action_build_hut.job);
      continue;
    }

    // There are no more agents within the hut bounds. Place the hut and remove the build hut action
    // First, remove preview placeholder
    for (const auto entity : registry.view<entt::tag<"hut_preview"_hs>>())
    {
      registry.destroy(entity);
    }

    assert(hut_size >= 3);

    // TODO: Use bitmasks to represent huts
    // Place hut structure tiles
    if (hut_size == 3)
    {
      // Top
      m_world.set_decoration(148, target.x + 1, target.y + 1, target.z + 1);

      // TODO: Add sprites for when top is not displayed
      m_world.set_decoration(148, target.x + 1, target.y, target.z);

      // Perimeter
      m_world.set_decoration(139, target.x, target.y, target.z);
      m_world.set_decoration(140, target.x + 1, target.y, target.z);
      m_world.set_decoration(141, target.x + 2, target.y, target.z);
      m_world.set_decoration(142, target.x, target.y + 1, target.z);
      m_world.set_decoration(144, target.x + 2, target.y + 1, target.z);
      m_world.set_decoration(145, target.x, target.y + 2, target.z);
      m_world.set_decoration(146, target.x + 1, target.y + 2, target.z);
      m_world.set_decoration(147, target.x + 2, target.y + 2, target.z);
    }
    else if (hut_size == 4)
    {
      // Perimeter
      m_world.set_decoration(149, target.x, target.y, target.z);
      m_world.set_decoration(155, target.x + 3, target.y, target.z);
      m_world.set_decoration(152, target.x, target.y + 1, target.z);
      m_world.set_decoration(153, target.x + 3, target.y + 1, target.z);
      m_world.set_decoration(175, target.x, target.y + 2, target.z);
      m_world.set_decoration(181, target.x + 3, target.y + 2, target.z);
      m_world.set_decoration(182, target.x, target.y + 3, target.z);
      m_world.set_decoration(183, target.x + 1, target.y + 3, target.z);
      m_world.set_decoration(187, target.x + 2, target.y + 3, target.z);
      m_world.set_decoration(188, target.x + 3, target.y + 3, target.z);

      // Top
      m_world.set_decoration(150, target.x + 1, target.y + 1, target.z + 1);
      m_world.set_decoration(154, target.x + 2, target.y + 1, target.z + 1);
      m_world.set_decoration(169, target.x + 1, target.y + 2, target.z + 1);
      m_world.set_decoration(173, target.x + 2, target.y + 2, target.z + 1);

      // TODO: Add sprites for when top is not displayed
      m_world.set_decoration(150, target.x + 1, target.y, target.z);
      m_world.set_decoration(154, target.x + 2, target.y, target.z);
    }
    else
    {
      // Perimeter structure parts
      m_world.set_decoration(149, target.x, target.y, target.z);
      m_world.set_decoration(155, target.x + hut_size - 1, target.y, target.z);
      m_world.set_decoration(156, target.x, target.y + 1, target.z);
      m_world.set_decoration(159, target.x + hut_size - 1, target.y + 1, target.z);

      m_world.set_decoration(168, target.x, target.y + hut_size - 3, target.z);
      m_world.set_decoration(175, target.x, target.y + hut_size - 2, target.z);
      m_world.set_decoration(174, target.x + hut_size - 1, target.y + hut_size - 3, target.z);
      m_world.set_decoration(181, target.x + hut_size - 1, target.y + hut_size - 2, target.z);
      m_world.set_decoration(182, target.x, target.y + hut_size - 1, target.z);
      m_world.set_decoration(183, target.x + 1, target.y + hut_size - 1, target.z);
      m_world.set_decoration(187, target.x + hut_size - 2, target.y + hut_size - 1, target.z);
      m_world.set_decoration(188, target.x + hut_size - 1, target.y + hut_size - 1, target.z);

      // Top structure parts
      m_world.set_decoration(150, target.x + 1, target.y + 1, target.z + 1);
      m_world.set_decoration(154, target.x + hut_size - 2, target.y + 1, target.z + 1);
      m_world.set_decoration(169, target.x + 1, target.y + hut_size - 2, target.z + 1);
      m_world.set_decoration(173, target.x + hut_size - 2, target.y + hut_size - 2, target.z + 1);

      // TODO: Draw specific sprites for when top is not displayed
      m_world.set_decoration(150, target.x + 1, target.y, target.z);
      m_world.set_decoration(154, target.x + hut_size - 2, target.y, target.z);

      // Tiled parts
      for (uint32_t i = 2; i < hut_size - 2; ++i)
      {
        // Horizontal structure parts
        m_world.set_decoration(151, target.x + i, target.y + 1, target.z + 1);
        m_world.set_decoration(170, target.x + i, target.y + hut_size - 2, target.z + 1);
        m_world.set_decoration(184, target.x + i, target.y + hut_size - 1, target.z);

        // TODO: Draw specific sprites for when top is not displayed
        m_world.set_decoration(151, target.x + i, target.y, target.z);

        // Vertical structure parts
        m_world.set_decoration(157, target.x + 1, target.y + i, target.z + 1);
        m_world.set_decoration(158, target.x + hut_size - 2, target.y + i, target.z + 1);

        if (i < hut_size - 3)
        {
          m_world.set_decoration(160, target.x, target.y + i, target.z);
          m_world.set_decoration(163, target.x + hut_size - 1, target.y + i, target.z);
        }
      }
    }

    // Create entities for the home floor
    const auto home_entity = registry.create();
    registry.emplace<Position>(home_entity, target.x, target.y, target.z);
    registry.emplace<Home>(home_entity, 0, Vector3i{static_cast<int>(hut_size), static_cast<int>(hut_size), 2});

    std::vector<entt::entity> home_floor(hut_size * hut_size);

    for (uint32_t j = 0; j < hut_size; ++j)
    {
      for (uint32_t i = 0; i < hut_size; ++i)
      {
        const uint32_t index = j * hut_size + i;
        home_floor[index] = registry.create();
        registry.emplace<Position>(home_floor[index], target.x + i, target.y + j, target.z);
        registry.emplace<HomeFloor>(home_floor[index], home_entity);
      }
    }

    stop_build_hut(registry, entity, action_build_hut.job);
    registry.destroy(job_data.progress_entity);
  }

  auto place_hut_view = registry.view<ActionPlaceHutExterior, Biology, const Position>();
  for (const auto entity : place_hut_view)
  {
    const auto& action = registry.get<ActionPlaceHutExterior>(entity);
    const auto& action_target = registry.get<Target>(action.job);
    const auto floor_entity = m_world.spatial_hash.get_by_component<HomeFloor>(
        action_target.position.x, action_target.position.y, action_target.position.z, registry);

    if (!registry.valid(floor_entity))
    {
      stop_place_exterior(registry, entity, action.job);
      continue;
    }

    const auto& home_floor = registry.get<HomeFloor>(floor_entity);
    const auto& home = registry.get<Home>(home_floor.home_entity);
    const auto& home_position = registry.get<Position>(home_floor.home_entity);

    assert(home.size.x == home.size.y);
    assert(home.size.x >= 3);

    const int hut_size = home.size.x;

    // TODO: Use bitmasks to represent huts
    switch (hut_size)
    {
    case 3:
      // Top
      m_world.set_decoration(199, home_position.x + 1, home_position.y + 1, home_position.z + 1);

      // TODO: Draw specific sprites for when top is not displayed
      m_world.set_decoration(199, home_position.x + 1, home_position.y, home_position.z);

      // Perimeter
      m_world.set_decoration(190, home_position.x, home_position.y, home_position.z);
      m_world.set_decoration(191, home_position.x + 1, home_position.y, home_position.z);
      m_world.set_decoration(192, home_position.x + 2, home_position.y, home_position.z);
      m_world.set_decoration(193, home_position.x, home_position.y + 1, home_position.z);
      m_world.set_decoration(195, home_position.x + 2, home_position.y + 1, home_position.z);
      m_world.set_decoration(196, home_position.x, home_position.y + 2, home_position.z);
      m_world.set_decoration(197, home_position.x + 1, home_position.y + 2, home_position.z);
      m_world.set_decoration(198, home_position.x + 2, home_position.y + 2, home_position.z);
      break;
    default:
      // Top
      m_world.set_decoration(201, home_position.x + 1, home_position.y + 1, home_position.z + 1);
      m_world.set_decoration(205, home_position.x + hut_size - 2, home_position.y + 1, home_position.z + 1);
      m_world.set_decoration(221, home_position.x + 1, home_position.y + hut_size - 2, home_position.z + 1);
      m_world.set_decoration(225, home_position.x + hut_size - 2, home_position.y + hut_size - 2, home_position.z + 1);

      // TODO: Draw specific sprites for when top is not displayed
      m_world.set_decoration(201, home_position.x + 1, home_position.y, home_position.z);
      m_world.set_decoration(205, home_position.x + hut_size - 2, home_position.y, home_position.z);

      // Place center top parts if needed
      if (hut_size > 4)
      {
        const int center_size = hut_size - 4;

        for (int j = 0; j < center_size; ++j)
        {
          for (int i = 0; i < center_size; ++i)
          {
            m_world.set_decoration(209, home_position.x + 2 + i, home_position.y + 2 + j, home_position.z + 1);
          }
        }
      }

      // Perimeter
      m_world.set_decoration(200, home_position.x, home_position.y, home_position.z);
      m_world.set_decoration(206, home_position.x + hut_size - 1, home_position.y, home_position.z);

      m_world.set_decoration(207, home_position.x, home_position.y + 1, home_position.z);
      m_world.set_decoration(211, home_position.x + hut_size - 1, home_position.y + 1, home_position.z);

      m_world.set_decoration(227, home_position.x, home_position.y + hut_size - 2, home_position.z);
      m_world.set_decoration(233, home_position.x + hut_size - 1, home_position.y + hut_size - 2, home_position.z);

      m_world.set_decoration(234, home_position.x, home_position.y + hut_size - 1, home_position.z);
      m_world.set_decoration(235, home_position.x + 1, home_position.y + hut_size - 1, home_position.z);
      m_world.set_decoration(239, home_position.x + hut_size - 2, home_position.y + hut_size - 1, home_position.z);
      m_world.set_decoration(240, home_position.x + hut_size - 1, home_position.y + hut_size - 1, home_position.z);

      // Tiled parts
      for (int i = 2; i < hut_size - 2; ++i)
      {
        // Horizontal structure parts
        m_world.set_decoration(202, home_position.x + i, home_position.y + 1, home_position.z + 1);
        m_world.set_decoration(222, home_position.x + i, home_position.y + hut_size - 2, home_position.z + 1);
        m_world.set_decoration(236, home_position.x + i, home_position.y + hut_size - 1, home_position.z);

        // TODO: Draw specific sprites for when top is not displayed
        m_world.set_decoration(202, home_position.x + i, home_position.y, home_position.z);

        // Vertical structure parts
        m_world.set_decoration(208, home_position.x + 1, home_position.y + i, home_position.z + 1);
        m_world.set_decoration(210, home_position.x + hut_size - 2, home_position.y + i, home_position.z + 1);
        m_world.set_decoration(212, home_position.x, home_position.y + i, home_position.z);
        m_world.set_decoration(215, home_position.x + hut_size - 1, home_position.y + i, home_position.z);
      }

      break;
    }

    stop_place_exterior(registry, entity, action.job);
  }
}

void BuildHutSystem::update_state(entt::registry& registry, const Camera& camera)
{
  switch (m_state)
  {
  case State::SelectHutTarget:
  {
    m_update_select_target(registry, camera);
    break;
  }
  default:
    break;
  }
}

void BuildHutSystem::m_update_select_target(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  if (!m_input_manager.is_context("action_menu"_hs))
  {
    m_input_manager.push_context("action_menu"_hs);
  }
  if (m_input_manager.poll_action("close_menu"_hs))
  {
    m_dispose();
    return;
  }

  if (m_notification == nullptr)
  {
    m_notification = m_ui_manager.notify("Select area for hut");
  }

  m_select_hut_target(registry, camera);
}

void BuildHutSystem::m_random_walk(entt::registry& registry, const entt::entity entity, const entt::entity job)
{
  // Don't walk if agent is already walking
  if (registry.all_of<ActionWalk>(entity))
  {
    return;
  }

  // Randomly walk during action
  const auto dice_roll = random::get_integer(0, 100);

  if (dice_roll < 20)
  {
    auto& job_data = registry.get<JobData>(job);
    job_data.status = JobStatus::Waiting;
    auto& job_data_build_hut = registry.get<JobDataBuildHut>(job);
    const uint32_t hut_size = job_data_build_hut.hut_size;
    const auto& job_position = registry.get<Position>(job_data.progress_entity);

    const auto offset_x = random::get_integer(0, hut_size);
    const auto offset_y = random::get_integer(0, hut_size);

    const auto new_position = Vector3i{job_position.x + offset_x, job_position.y + offset_y, job_position.z};

    action::walk::job({
        .registry = registry,
        .agent_entity = entity,
        .position = new_position,
        .priority = 0,
    });
  }
}

bool BuildHutSystem::m_is_within_hut_bounds(const Position& agent_position,
                                            const Position& hut_position,
                                            const uint32_t hut_size)
{
  return agent_position.x >= hut_position.x && agent_position.x < hut_position.x + hut_size
         && agent_position.y >= hut_position.y && agent_position.y < hut_position.y + hut_size;
}

void BuildHutSystem::m_select_hut_target(entt::registry& registry, const Camera& camera)
{
  const auto& drag_bounds = m_input_manager.get_drag_bounds();

  Vector3i begin = m_world.screen_to_world(Vector2i{drag_bounds.x, drag_bounds.y}, camera);
  Vector3i end = m_world.screen_to_world(Vector2i{drag_bounds.z, drag_bounds.w}, camera);
  Vector2i direction{begin.x <= end.x ? 0 : 1, begin.y <= end.y ? 0 : 1};
  Vector2i area{std::abs(end.x - (begin.x)), std::abs(end.y - (begin.y))};

  const uint32_t hut_size = std::max(std::max(area.x + 1, area.y + 1), 3);

  // Change where we start rendering the hut depending on the drag direction
  begin.x -= (hut_size - 1) * direction.x;
  begin.y -= (hut_size - 1) * direction.y;

  if (m_input_manager.is_dragging())
  {
    if (m_last_hut_size != hut_size || m_last_direction != direction)
    {
      m_last_hut_size = hut_size;
      m_last_direction = direction;
      m_preview_hut_target(begin, hut_size, registry);
    }
  }
  else if (m_input_manager.has_dragged())
  {
    m_last_hut_size = 0;
    m_create_hut_job(begin, hut_size, registry);
  }
}

void BuildHutSystem::m_preview_hut_target(const Vector3i& position, const uint32_t hut_size, entt::registry& registry)
{
  using namespace entt::literals;

  assert(hut_size >= 3);

  for (const auto entity : registry.view<entt::tag<"hut_preview"_hs>>())
  {
    registry.destroy(entity);
  }

  const auto texture_id = m_world.get_spritesheet_id();

  auto add_hut_part
      = [&registry, texture_id](
            const uint32_t id, const int x, const int y, const int z, const Color& color = Color{0xFFFFFF99})
  {
    const auto entity = registry.create();
    registry.emplace<entt::tag<"hut_preview"_hs>>(entity);
    const auto sprite = Sprite{
        .id = id,
        .resource_id = texture_id,
        .layer_z = 4,
        .category = "tile",
        .color = color,
    };
    registry.emplace<Sprite>(entity, sprite);
    registry.emplace<Position>(entity, x, y, z);
  };

  Color preview_tile_color{0x66EEAA77};

  if (!m_can_build_hut(hut_size, position))
  {
    preview_tile_color = Color{0xEE664477};
  }

  for (uint32_t j = 0; j < hut_size; ++j)
  {
    for (uint32_t i = 0; i < hut_size; ++i)
    {
      add_hut_part(2, position.x + i, position.y + j, position.z, preview_tile_color);
    }
  }

  // TODO: Use bitmasks to represent huts
  if (hut_size == 3)
  {
    // Perimeter
    add_hut_part(139, position.x, position.y, position.z);
    add_hut_part(140, position.x + 1, position.y, position.z);
    add_hut_part(141, position.x + 2, position.y, position.z);
    add_hut_part(142, position.x, position.y + 1, position.z);
    add_hut_part(144, position.x + 2, position.y + 1, position.z);
    add_hut_part(145, position.x, position.y + 2, position.z);
    add_hut_part(146, position.x + 1, position.y + 2, position.z);
    add_hut_part(147, position.x + 2, position.y + 2, position.z);

    // Top
    add_hut_part(148, position.x + 1, position.y + 1, position.z + 1);

    // TODO: Automatically add bottom tile parts
    add_hut_part(143, position.x + 1, position.y + 1, position.z);
  }
  else if (hut_size == 4)
  {
    // Perimeter
    add_hut_part(149, position.x, position.y, position.z);
    add_hut_part(155, position.x + 3, position.y, position.z);
    add_hut_part(152, position.x, position.y + 1, position.z);
    add_hut_part(153, position.x + 3, position.y + 1, position.z);
    add_hut_part(175, position.x, position.y + 2, position.z);
    add_hut_part(181, position.x + 3, position.y + 2, position.z);
    add_hut_part(182, position.x, position.y + 3, position.z);
    add_hut_part(183, position.x + 1, position.y + 3, position.z);
    add_hut_part(187, position.x + 2, position.y + 3, position.z);
    add_hut_part(188, position.x + 3, position.y + 3, position.z);

    // Top
    add_hut_part(150, position.x + 1, position.y + 1, position.z + 1);
    add_hut_part(154, position.x + 2, position.y + 1, position.z + 1);
    add_hut_part(169, position.x + 1, position.y + 2, position.z + 1);
    add_hut_part(173, position.x + 2, position.y + 2, position.z + 1);

    // TODO: Automatically add bottom tile parts
    add_hut_part(176, position.x + 1, position.y + 2, position.z);
    add_hut_part(180, position.x + 2, position.y + 2, position.z);
  }
  else
  {
    // Perimeter structure parts
    add_hut_part(149, position.x, position.y, position.z);
    add_hut_part(155, position.x + hut_size - 1, position.y, position.z);
    add_hut_part(156, position.x, position.y + 1, position.z);
    add_hut_part(159, position.x + hut_size - 1, position.y + 1, position.z);

    add_hut_part(168, position.x, position.y + hut_size - 3, position.z);
    add_hut_part(175, position.x, position.y + hut_size - 2, position.z);
    add_hut_part(174, position.x + hut_size - 1, position.y + hut_size - 3, position.z);
    add_hut_part(181, position.x + hut_size - 1, position.y + hut_size - 2, position.z);
    add_hut_part(182, position.x, position.y + hut_size - 1, position.z);
    add_hut_part(183, position.x + 1, position.y + hut_size - 1, position.z);
    add_hut_part(187, position.x + hut_size - 2, position.y + hut_size - 1, position.z);
    add_hut_part(188, position.x + hut_size - 1, position.y + hut_size - 1, position.z);

    // Top structure parts
    add_hut_part(150, position.x + 1, position.y + 1, position.z + 1);
    add_hut_part(154, position.x + hut_size - 2, position.y + 1, position.z + 1);
    add_hut_part(169, position.x + 1, position.y + hut_size - 2, position.z + 1);
    add_hut_part(173, position.x + hut_size - 2, position.y + hut_size - 2, position.z + 1);

    // TODO: Automatically add bottom tile parts
    add_hut_part(176, position.x + 1, position.y + hut_size - 2, position.z);
    add_hut_part(180, position.x + hut_size - 2, position.y + hut_size - 2, position.z);

    // Tiled parts
    for (int i = 2; i < static_cast<int>(hut_size) - 2; ++i)
    {
      // Horizontal structure parts
      add_hut_part(151, position.x + i, position.y + 1, position.z + 1);
      add_hut_part(170, position.x + i, position.y + hut_size - 2, position.z + 1);
      add_hut_part(184, position.x + i, position.y + hut_size - 1, position.z);

      // Vertical structure parts
      add_hut_part(157, position.x + 1, position.y + i, position.z + 1);
      add_hut_part(158, position.x + hut_size - 2, position.y + i, position.z + 1);

      if (i < static_cast<int>(hut_size) - 3)
      {
        add_hut_part(160, position.x, position.y + i, position.z);
        add_hut_part(163, position.x + hut_size - 1, position.y + i, position.z);
      }

      // TODO: Automatically add bottom tile parts
      add_hut_part(177, position.x + i, position.y + hut_size - 2, position.z);
    }
  }
}

void BuildHutSystem::m_create_hut_job(const Vector3i& position, const uint32_t hut_size, entt::registry& registry)
{
  using namespace entt::literals;

  if (!m_can_build_hut(hut_size, position))
  {
    for (const auto entity : registry.view<entt::tag<"hut_preview"_hs>>())
    {
      registry.destroy(entity);
    }
    return;
  }

  auto entities = m_select_available_entities(registry);

  action::create_hut::job({
      .registry = registry,
      .entities = entities,
      .position = position,
      .hut_size = hut_size,
  });

  m_dispose();
}

bool BuildHutSystem::m_can_build_hut(const uint32_t hut_size, const Vector3i& position)
{
  if (hut_size < 3)
  {
    return false;
  }

  // Check if soil is buildable
  for (uint32_t j = 0; j < hut_size; ++j)
  {
    for (uint32_t i = 0; i < hut_size; ++i)
    {
      if (!m_world.is_walkable(position.x + i, position.y + j, position.z))
      {
        return false;
      }
    }
  }

  // Check if there's space over the ground to build the hut
  for (uint32_t j = 0; j < hut_size; ++j)
  {
    for (uint32_t i = 0; i < hut_size; ++i)
    {
      if (!m_world.is_empty(position.x + i, position.y + j, position.z + 1))
      {
        return false;
      }
    }
  }

  return true;
}

std::vector<entt::entity> BuildHutSystem::m_select_available_entities(entt::registry& registry)
{
  std::vector<entt::entity> free_entities;
  std::vector<entt::entity> selected_entities;

  for (const auto entity : registry.view<SocietyAgent, Selectable>())
  {
    const auto& agent = registry.get<SocietyAgent>(entity);
    const auto& selectable = registry.get<Selectable>(entity);

    if (selectable.selected)
    {
      selected_entities.push_back(entity);
    }

    if (!agent.jobs.empty())
    {
      continue;
    }

    free_entities.push_back(entity);
  }

  if (!selected_entities.empty())
  {
    return selected_entities;
  }

  return free_entities;
}

void BuildHutSystem::m_dispose()
{
  m_input_manager.pop_context();

  if (m_notification != nullptr)
  {
    m_notification->hide();
    m_notification = nullptr;
  }

  m_state = State::None;
}

}  // namespace dl
