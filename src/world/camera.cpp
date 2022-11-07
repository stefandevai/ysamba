#include "./camera.hpp"

#include <algorithm>

namespace dl
{
  Camera::Camera() { }

  void Camera::update(const Position& target, const Size& tilemap_size)
  {
    const int camera_x = target.x - size.w / 2;
    const int camera_y = target.y - size.h / 2;

    position.x = std::min(std::max(camera_x, 0), tilemap_size.w - size.w);
    position.y = std::min(std::max(camera_y, 0), tilemap_size.h - size.h);

    // Fix camera movement when target is close to bounds
    if (position.x != camera_x && !m_fixed_x)
    {
      m_fixed_x = true;
    }
    else if (m_fixed_x && position.x == camera_x)
    {
      m_fixed_x = false;
    }

    if (position.y != camera_y && !m_fixed_y)
    {
      m_fixed_y = true;
    }
    else if (m_fixed_y && position.y == camera_y)
    {
      m_fixed_y = false;
    }
  }

  void Camera::move(const int x, const int y, const int z)
  {
    if (x == position.x && y == position.y && z == position.z)
    {
      return;
    }

    position.x = x;
    position.y = y;
    position.z = z;
  }
}
