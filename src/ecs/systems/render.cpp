#include "./render.hpp"
#include "../components/position.hpp"
#include "../components/visibility.hpp"

namespace dl
{
  RenderSystem::RenderSystem()
  {

  }

  void RenderSystem::update(entt::registry &registry, TCOD_Console& console)
  {
    auto view = registry.view<const Position, const Visibility>();
    view.each([&console](const auto &position, const auto &visibility) { 
      console.at(position.x, position.y).ch = visibility.ch;
      console.at(position.x, position.y).fg = { visibility.r, visibility.g, visibility.b, 255 };
    });
  }
}

