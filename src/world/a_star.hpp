#pragma once

#include <memory>
#include <vector>

#include "core/maths/vector.hpp"
#include "definitions.hpp"

#ifdef DL_BUILD_DEBUG_TOOLS
#include <entt/fwd.hpp>
#endif

namespace dl
{
class World;
template <typename T>
class NeighborIterator;

class AStar
{
 private:
  World& m_world;

 public:
  enum class State
  {
    NONE,
    INITIALIZED,
    SEARCHING,
    SUCCEEDED,
    FAILED,
  };

  struct Node
  {
    Vector3i position;
    Node* parent = nullptr;
    int f = 0;
    int g = 0;
    int h = 0;
  };

  State state = State::NONE;
  Vector3i origin;
  Vector3i destination;
  std::size_t steps = 0;
  std::shared_ptr<std::vector<Vector3i>> path{};

  AStar(World& world, const Vector3i& origin, const Vector3i& destination);

  void step();

#ifdef DL_BUILD_DEBUG_TOOLS
  // Draws rectangles for the open set, closed set and path
  void debug(entt::registry& registry, const bool only_path = true, const bool clear_previous = true);
#endif

  std::vector<Node> m_open_set{};
  std::vector<std::shared_ptr<Node>> m_closed_set{};

 private:
  int m_get_cost(const Vector3i& current, const Vector3i& neighbor, const bool is_diagonal) const;
};
}  // namespace dl
