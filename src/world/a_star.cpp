#include "./a_star.hpp"

#include <spdlog/spdlog.h>

#include <cmath>

#include "constants.hpp"
#include "core/maths/neighbor_iterator.hpp"
#include "world/world.hpp"

namespace
{
int distance_squared(const dl::Vector3i& a, const dl::Vector3i& b)
{
  // const int distance_x = std::abs(a.x - b.x);
  // const int distance_y = std::abs(a.y - b.y);
  // const int distance_z = std::abs(a.z - b.z);
  //
  // if (distance_x > distance_y)
  // {
  //   return 141 * distance_y + 100 * (distance_x - distance_y) + 200 * distance_z;
  // }
  //
  // return 141 * distance_x + 100 * (distance_y - distance_x) + 200 * distance_z;

  // Euclidean squared
  // return (std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) + std::pow(a.z - b.z, 2)) * 100;

  // Euclidean
  return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) + std::pow(a.z - b.z, 2)) * 100;

  // Manhattan
  // return (std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z)) * 100;
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
    spdlog::info("Astar initialized. FROM: {} {} {} TO: {} {} {}",
                 origin.x,
                 origin.y,
                 origin.z,
                 destination.x,
                 destination.y,
                 destination.z);
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
    spdlog::debug("AStar failed to find a path");
    state = State::FAILED;
    return;
  }

  ++steps;

  auto current_node = m_open_set.front();

  spdlog::info("Searching ({}, {}, {}) f: {} g: {} h: {}",
               current_node.position.x,
               current_node.position.y,
               current_node.position.z,
               current_node.f,
               current_node.g,
               current_node.h);

  // Found the path
  if (current_node.position == destination)
  {
    spdlog::warn("Success! Found a path to the destination! Steps: {}", steps);
    state = State::SUCCEEDED;
    path = std::make_shared<std::vector<Vector3i>>();
    path->push_back(current_node.position);

    while (current_node.parent != nullptr)
    {
      path->push_back(current_node.parent->position);
      current_node = *current_node.parent;
    }
    std::reverse(path->begin(), path->end());
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
    const int h = distance_squared(neighbor, destination);
    const bool walkable = m_world.is_walkable(neighbor.x, neighbor.y, neighbor.z);

    if (!walkable)
    {
      continue;
    }

    // // If neighbor is not walkable and it's further away from the destination than the current node, skip it
    // if (!walkable && h > current_node.h)
    // {
    //   continue;
    // }
    // // If neighbor is not walkable but it's closer to the destination than the current node, check if we can climb
    // // up or down. If we can, set the neighbor to the new position, otherwise skip to the next neighbor
    // else if (!walkable && h < current_node.h)
    // {
    //   const bool can_climb_up = m_world.is_walkable(neighbor.x, neighbor.y, neighbor.z + 1);
    //   const bool can_climb_down = m_world.is_walkable(neighbor.x, neighbor.y, neighbor.z - 1);
    //
    //   if (can_climb_up)
    //   {
    //     neighbor = {neighbor.x, neighbor.y, neighbor.z + 1};
    //   }
    //   else if (can_climb_down)
    //   {
    //     neighbor = {neighbor.x, neighbor.y, neighbor.z - 1};
    //   }
    //   else
    //   {
    //     continue;
    //   }
    // }

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
    // cost += pathfinding::diagonal_cost_penalty;
    cost += 41;
  }

  // if (current.z != neighbor.z)
  // {
  //   // Increase cost for climbing
  //   if (neighbor.z > current.z)
  //   {
  //     // cost += pathfinding::climb_up_cost_penalty;
  //     cost += pathfinding::climb_up_cost_penalty;
  //   }
  //   else if (neighbor.z < current.z)
  //   {
  //     // cost += pathfinding::climb_down_cost_penalty;
  //     cost += pathfinding::climb_down_cost_penalty;
  //   }
  // }

  return cost;
}
}  // namespace dl
