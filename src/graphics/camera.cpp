#include "./camera.hpp"

#include <cmath>
#include <entt/entt.hpp>

namespace dl
{
ViewCamera::ViewCamera() { m_target = entt::null; }

void ViewCamera::update(entt::registry& registry)
{
  if (m_target == entt::null)
  {
    return;
  }
}

void ViewCamera::move(const float x, const float y, const float z)
{
  m_position.x += x;
  m_position.y += y;
  m_position.z += z;

  m_front.x += x;
  m_front.y += y;
  m_front.z += z;
}

void ViewCamera::set_position(const float x, const float y, const float z)
{
  m_position.x = x;
  m_position.y = y;
  m_position.z = z;

  m_front.x = x;
  m_front.y = y;
  m_front.z = z - 501;
}

void ViewCamera::set_frustrum(const float left, const float right, const float bottom, const float top)
{
  m_dimensions.x = std::abs(right - left);
  m_dimensions.y = std::abs(top - bottom);
  m_frustrum_left = left;
  m_frustrum_right = right;
  m_frustrum_bottom = bottom;
  m_frustrum_top = top;
}

void ViewCamera::set_target(const entt::entity& target) { m_target = target; }

void ViewCamera::remove_target() { m_target = entt::null; }
}  // namespace dl
