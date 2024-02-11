#include "./a_star.hpp"

#include <spdlog/spdlog.h>

#include <cmath>

#include "core/maths/neighbor_iterator.hpp"
#include "world/world.hpp"

namespace
{
int distance_squared(const dl::Vector3i& a, const dl::Vector3i& b)
{
  return std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2);
}
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

  m_open_set.push_back(Node{origin, nullptr, 0, 0, 0});
  state = State::INITIALIZED;
}

void AStar::step()
{
  // AStar has not yet been initialized
  if (state == State::NONE)
  {
    spdlog::debug("Astar has not been initialized");
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
    spdlog::debug("Search has already finished");
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

  // Found the path
  if (current_node.position == destination)
  {
    spdlog::debug("Success! Found a path to the destination!");
    state = State::SUCCEEDED;
    path = std::make_shared<std::vector<Vector3i>>();
    path->push_back(current_node.position);
    spdlog::debug("Adding to path... {} {} {} {}",
                  current_node.position.x,
                  current_node.position.y,
                  current_node.position.z,
                  current_node.parent != nullptr);

    while (current_node.parent != nullptr)
    {
      path->push_back(current_node.parent->position);
      current_node = *current_node.parent;
      spdlog::debug("Adding to path... {} {} {} {}",
                    current_node.position.x,
                    current_node.position.y,
                    current_node.position.z,
                    current_node.parent != nullptr);
    }
    std::reverse(path->begin(), path->end());
    return;
  }

  std::pop_heap(m_open_set.begin(), m_open_set.end(), [](const auto& a, const auto& b) { return a.f > b.f; });
  m_open_set.pop_back();

  if (current_node.parent == nullptr)
  {
    spdlog::debug("Adding to close set {} {} {} parent: nullptr",
                  current_node.position.x,
                  current_node.position.y,
                  current_node.position.z);
  }
  else
  {
    spdlog::debug("Adding to close set {} {} {} parent: {} {} {} {}",
                  current_node.position.x,
                  current_node.position.y,
                  current_node.position.z,
                  current_node.parent->position.x,
                  current_node.parent->position.y,
                  current_node.parent->position.z,
                  current_node.parent->parent == nullptr);
  }
  m_closed_set.push_back(std::make_shared<Node>(current_node));

  NeighborIterator<Vector3i> it{current_node.position};

  spdlog::info(
      "Searching center at {} {} {}", current_node.position.x, current_node.position.y, current_node.position.z);
  for (; it.neighbor != 8; ++it)
  {
    const auto neighbor = *it;
    // spdlog::debug("Checking neighbor {} at {} {} {}", it.neighbor, neighbor.x, neighbor.y, neighbor.z);

    if (!m_world.is_walkable(neighbor.x, neighbor.y, neighbor.z))
    {
      // spdlog::debug("Not walkable...");
      continue;
    }

    if (std::find_if(m_closed_set.begin(), m_closed_set.end(), [&neighbor](const auto& node) {
          return node->position == neighbor;
        }) != m_closed_set.end())
    {
      // spdlog::debug("Found in closed...");
      continue;
    }

    const int g = current_node.g + m_get_cost(current_node.position, it);
    const int h = distance_squared(neighbor, destination);
    const int f = g + h;

    auto open_it = std::find_if(
        m_open_set.begin(), m_open_set.end(), [&neighbor](const auto& node) { return node.position == neighbor; });

    if (open_it == m_open_set.end())
    {
      auto& closed_node = m_closed_set[m_closed_set.size() - 1];

      // spdlog::debug("Adding to open set {} {} {} parent: {} {} {}", neighbor.x, neighbor.y, neighbor.z,
      // closed_node.position.x, closed_node.position.y, closed_node.position.z);
      m_open_set.push_back(Node{neighbor, closed_node.get(), f, g, h});
      std::push_heap(m_open_set.begin(), m_open_set.end(), [](const auto& a, const auto& b) { return a.f > b.f; });
    }
    else if (g < open_it->g)
    {
      // spdlog::warn("Updating open set... g: {} f: {}", g, f);
      open_it->g = g;
      open_it->f = f;
      open_it->parent = m_closed_set.back().get();
    }
  }
}

int AStar::m_get_cost(const Vector3i& current, NeighborIterator<Vector3i>& neighbor) const
{
  // Base cost
  int cost = 100;

  // Increase cost for diagonal movement
  if (neighbor.is_diagonal)
  {
    cost += 41;
  }

  // Increase cost for climbing
  if ((*neighbor).z > current.z)
  {
    cost += 30;
  }

  return cost;
}
}  // namespace dl
