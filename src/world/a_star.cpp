#include "./a_star.hpp"

#include <spdlog/spdlog.h>

#include <cmath>

#include "config.hpp"
#include "constants.hpp"
#include "core/maths/neighbor_iterator.hpp"
#include "world/world.hpp"

#ifdef DL_BUILD_DEBUG_TOOLS
#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include "ecs/components/rectangle.hpp"
#endif

namespace
{
int distance(const dl::Vector3i& a, const dl::Vector3i& b)
{
  constexpr float scale = 1.2f;
  constexpr int normal_cost = 100 * scale;
  constexpr int diagonal_cost = 141 * scale;
  // constexpr int diagonal_up_cost = 300 * scale;
  // constexpr int diagonal_down_cost = 200 * scale;

  // Euclidean squared
  // return (std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) + std::pow(a.z - b.z, 2)) * normal_cost;

  // Euclidean
  // return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) + std::pow(a.z - b.z, 2)) * normal_cost;

  // Manhattan
  // return (std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z)) * normal_cost;

  // Manhattan with diagonal movement
  // const int dx = std::abs(a.x - b.x);
  // const int dy = std::abs(a.y - b.y);
  // const int dz = std::abs(a.z - b.z);
  // return normal_cost * (dx + dy + dz) + (diagonal_cost - 2 * normal_cost) * std::min(dx, dy);

  // Octile 2D
  const int dx = std::abs(a.x - b.x);
  const int dy = std::abs(a.y - b.y);
  return normal_cost * (dx + dy) + (diagonal_cost - 2 * normal_cost) * std::min(dx, dy);

  // Octile 3D
  // const int dx = std::abs(a.x - b.x);
  // const int dy = std::abs(a.y - b.y);
  // const int dz = std::abs(a.z - b.z);
  // const int dmin = std::min(std::min(dx, dy), dz);
  // const int dmax = std::max(std::max(dx, dy), dz);
  // const int dmid = dx + dy + dz - dmin - dmax;
  //
  // int result = 0;
  //
  // // Going up
  // if (b.z > a.z)
  // {
  //   result = (diagonal_up_cost - diagonal_cost) * dmin + (diagonal_cost - normal_cost) * dmid + normal_cost * dmax;
  // }
  // // Going down
  // else if (b.z < a.z)
  // {
  //   result = (diagonal_down_cost - diagonal_cost) * dmin + (diagonal_cost - normal_cost) * dmid + normal_cost * dmax;
  // }
  // // Same z level
  // else
  // {
  //   result = normal_cost * (dx + dy) + (diagonal_cost - 2 * normal_cost) * std::min(dx, dy);
  // }
  //
  // return result;
}

bool node_compare(const dl::AStar::Node& a, const dl::AStar::Node& b) { return a.f > b.f || (a.f == b.f && a.h > b.h); }
}  // namespace

namespace dl
{
AStar::AStar(World& world, const Vector3i& origin, const Vector3i& destination)
    : m_world(world), origin(origin), destination(destination)
{
  if (origin == destination)
  {
    state = State::SUCCEEDED;
    return;
  }

  if (!m_world.is_walkable(destination.x, destination.y, destination.z))
  {
    state = State::FAILED;
    return;
  }

  m_open_set.push_back(Node{origin, nullptr, 0, 0, 0});
  state = State::INITIALIZED;
}

void AStar::step()
{
  // AStar has not yet been initialized
  if (state == State::NONE)
  {
    return;
  }

  // If it was initialized, set the state to searching
  if (state == State::INITIALIZED)
  {
    state = State::SEARCHING;
  }

  // Allow stepping even after the search has finished
  if (state != State::SEARCHING)
  {
    return;
  }

  // Failed to find a path
  if (m_open_set.empty())
  {
    spdlog::warn("A* failed to find a path");
    state = State::FAILED;
    return;
  }

  ++steps;

  if (steps > config::pathfinding::max_steps)
  {
    spdlog::warn("Maximum steps for A* reached, aborting search");
    state = State::FAILED;
    return;
  }

  auto current_node = m_open_set.front();

  // Found the path
  if (current_node.position == destination)
  {
    state = State::SUCCEEDED;
    path.push_back(current_node.position);

    while (current_node.parent != nullptr)
    {
      path.push_back(current_node.parent->position);
      current_node = *current_node.parent;
    }

    return;
  }

  std::pop_heap(m_open_set.begin(), m_open_set.end(), node_compare);
  m_open_set.pop_back();

  m_closed_set.push_back(std::make_shared<Node>(current_node));

  // Iterate through the 8 2D neighbors of the current node clockwise starting from the top left
  NeighborIterator<Vector3i> it{current_node.position};

  for (; it.neighbor != 8; ++it)
  {
    auto neighbor = *it;
    const int h = distance(neighbor, destination);
    const bool walkable = m_world.is_walkable(neighbor.x, neighbor.y, neighbor.z);

    // If neighbor is not walkable and it's further away from the destination than the current node, skip it
    if (!walkable && h > current_node.h)
    {
      continue;
    }
    // If neighbor is not walkable but it's closer to the destination than the current node, check if we can climb
    // up or down. If we can, set the neighbor to the new position, otherwise skip to the next neighbor
    else if (!walkable && h < current_node.h)
    {
      const bool can_climb_up = m_world.is_walkable(neighbor.x, neighbor.y, neighbor.z + 1);
      const bool can_climb_down = m_world.is_walkable(neighbor.x, neighbor.y, neighbor.z - 1);

      if (can_climb_up)
      {
        neighbor = {neighbor.x, neighbor.y, neighbor.z + 1};
      }
      else if (can_climb_down)
      {
        neighbor = {neighbor.x, neighbor.y, neighbor.z - 1};
      }
      else
      {
        continue;
      }
    }

    // Skip if node is in the closed set
    const auto closed_it = std::find_if(
        m_closed_set.begin(), m_closed_set.end(), [&neighbor](const auto& node) { return node->position == neighbor; });

    if (closed_it != m_closed_set.end())
    {
      continue;
    }

    const int g = current_node.g + m_get_cost(current_node.position, neighbor, it.is_diagonal);
    const int f = g + h;

    auto open_it = std::find_if(
        m_open_set.begin(), m_open_set.end(), [&neighbor](const auto& node) { return node.position == neighbor; });

    if (open_it == m_open_set.end())
    {
      auto& closed_node = m_closed_set[m_closed_set.size() - 1];

      m_open_set.push_back(Node{neighbor, closed_node.get(), f, g, h});
      std::push_heap(m_open_set.begin(), m_open_set.end(), node_compare);
    }
    else if (g < open_it->g)
    {
      open_it->g = g;
      open_it->f = f;
      open_it->parent = m_closed_set.back().get();
    }
  }
}

int AStar::m_get_cost(const Vector3i& current, const Vector3i& neighbor, const bool is_diagonal) const
{
  // TODO: Get base cost from tile data
  int cost = 100;

  // Increase cost for diagonal movement
  if (is_diagonal)
  {
    cost += pathfinding::diagonal_cost_penalty;
  }

  if (current.z != neighbor.z)
  {
    // Increase cost for climbing
    if (neighbor.z > current.z)
    {
      cost += cost * 2;
    }
    else if (neighbor.z < current.z)
    {
      cost += cost;
    }
  }

  return cost;
}

#ifdef DL_BUILD_DEBUG_TOOLS
// Draws rectangles for the open set, closed set and path
void AStar::debug(entt::registry& registry, const bool only_path, const bool clear_previous)
{
  using namespace entt::literals;

  if (clear_previous)
  {
    for (const auto entity : registry.view<entt::tag<"a_star_rectangle"_hs>>())
    {
      registry.destroy(entity);
    }
  }

  if (!only_path)
  {
    for (const auto& step : m_open_set)
    {
      auto rect = registry.create();
      auto& r = registry.emplace<Rectangle>(rect, 16, 16, 0x1144cc88);
      r.z_index = 4;
      registry.emplace<Position>(rect, static_cast<double>(step.position.x), static_cast<double>(step.position.y), static_cast<double>(step.position.z));
      registry.emplace<entt::tag<"a_star_rectangle"_hs>>(rect);
    }
    for (const auto& step : m_closed_set)
    {
      auto rect = registry.create();
      auto& r = registry.emplace<Rectangle>(rect, 16, 16, 0x11cc4488);
      r.z_index = 4;
      registry.emplace<Position>(rect,
                                 static_cast<double>(step->position.x),
                                 static_cast<double>(step->position.y),
                                 static_cast<double>(step->position.z));

      registry.emplace<entt::tag<"a_star_rectangle"_hs>>(rect);
    }
  }
  for (const auto& step : path)
  {
    auto rect = registry.create();
    auto& r = registry.emplace<Rectangle>(rect, 16, 16, 0xcc441188);
    r.z_index = 4;
    registry.emplace<Position>(rect,
                               static_cast<double>(step.x),
                               static_cast<double>(step.y),
                               static_cast<double>(step.z));
    registry.emplace<entt::tag<"a_star_rectangle"_hs>>(rect);
  }
}
#endif
}  // namespace dl
