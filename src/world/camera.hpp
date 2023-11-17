#pragma once

#include "../ecs/components/position.hpp"
#include "../components/size.hpp"

namespace dl
{
  class Camera
  {
    public:
      Position position{0.,0.,0.};
      Size size;

      Camera();
      void follow(const Position& target, const Size& tilemap_size);
      void move_to(const int x, const int y, const int z);
      inline void move_west() { position.x -= 1.; }
      inline void move_east() { position.x += 1.; }
      inline void move_south() { position.y += 1.; }
      inline void move_north() { position.y -= 1.; }
      inline bool is_fixed_x() const { return m_fixed_x; }
      inline bool is_fixed_y() const { return m_fixed_y; }

    private:
      bool m_fixed_x = false;
      bool m_fixed_y = false;
  };
}
