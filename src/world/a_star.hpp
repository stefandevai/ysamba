#pragma once

#include <memory>
#include <vector>

#include "core/maths/vector.hpp"

namespace dl
{
class World;

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

  State state = State::NONE;
  Vector3i origin;
  Vector3i destination;
  std::size_t steps = 0;
  std::shared_ptr<std::vector<Vector3i>> path{};

  AStar(World& world, const Vector3i& origin, const Vector3i& destination);

  void step();

 private:
  struct Node
  {
    Vector3i position;
    Node* parent = nullptr;
    int f = 0;
    int g = 0;
    int h = 0;
  };

  std::vector<Node> m_open_set{};
  std::vector<std::shared_ptr<Node>> m_closed_set{};
};
}  // namespace dl
