#include <tgmath.h>
#include "./render.hpp"
#include "../../world/camera.hpp"
#include "../components/position.hpp"
#include "../components/visibility.hpp"

namespace dl
{
  RenderSystem::RenderSystem()
  {

  }

  void RenderSystem::update(entt::registry &registry, TCOD_Console& console, const Camera& camera)
  {
    auto view = registry.view<const Position, const Visibility>();
    view.each([&console, camera](const auto &position, const auto &visibility) { 
      const auto position_x = static_cast<int>(std::round(position.x - camera.position.x));
      const auto position_y = static_cast<int>(std::round(position.y - camera.position.y));

      if (!console.in_bounds({position_x, position_y}))
      {
        return;
      }
      console.at(position.x - camera.position.x, position.y - camera.position.y).ch = visibility.ch;
      console.at(position.x - camera.position.x, position.y - camera.position.y).fg = { visibility.r, visibility.g, visibility.b, 255 };
    });
  }
}

