#pragma once

#include "../components/position.hpp"
#include "../components/size.hpp"

namespace dl
{
  class Camera
  {
    public:
      Position position;
      Size size;

      Camera();
      void update(const Position& target, const Size& tilemap_size);
      void move(const int x, const int y, const int z);
      inline bool is_fixed_x() const { return m_fixed_x; }
      inline bool is_fixed_y() const { return m_fixed_y; }

    private:
      bool m_fixed_x = false;
      bool m_fixed_y = false;
  };
}
