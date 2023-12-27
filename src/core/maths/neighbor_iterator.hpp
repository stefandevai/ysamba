#pragma once

#include <array>
#include <cstdint>
#include <iterator>

namespace dl
{
// Iterator to traverse the neighbors of a Vector clockwise starting from the top left neighbor
template <typename T>
class NeighborIterator
{
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using pointer = const T*;
  using reference = const T&;

  uint32_t neighbor = 0;
  reference center;

  NeighborIterator(reference center) : center(center) {}

  NeighborIterator(const NeighborIterator& other) : center(other.center) { neighbor = other.neighbor; }

  const NeighborIterator& operator=(const NeighborIterator& rhs)
  {
    center = rhs.center;
    neighbor = rhs.neighbor;
    return *this;
  }

  const NeighborIterator& operator=(const T& rhs)
  {
    center = rhs;
    return *this;
  }

  T operator*()
  {
    T value{};
    value.x = center.x + m_x_offsets[neighbor];
    value.y = center.y + m_y_offsets[neighbor];
    return value;
  }

  NeighborIterator& operator++()
  {
    if (neighbor == 8)
    {
      return *this;
    }

    ++neighbor;
    return *this;
  }

  NeighborIterator operator++(int)
  {
    NeighborIterator temp = *this;
    ++(*this);
    return temp;
  }

  NeighborIterator& operator--()
  {
    if (neighbor == 0)
    {
      return *this;
    }

    --neighbor;
    return *this;
  }

  NeighborIterator operator--(int)
  {
    NeighborIterator temp = *this;
    --(*this);
    return temp;
  }

  bool operator==(const NeighborIterator& rhs) const { return center == rhs.center; }

  bool operator!=(const NeighborIterator& rhs) const { return center != rhs.center; }

 private:
  static constexpr std::array<int, 8> m_x_offsets{-1, 0, 1, 1, 1, 0, -1, -1};
  static constexpr std::array<int, 8> m_y_offsets{-1, -1, -1, 0, 1, 1, 1, 0};
};

}  // namespace dl
