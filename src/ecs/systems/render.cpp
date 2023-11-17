#include <tgmath.h>
#include <iostream>
#include "./render.hpp"
#include "../../world/camera.hpp"
#include "../components/position.hpp"
#include "../components/visibility.hpp"
#include "../../graphics/renderer.hpp"

namespace dl
{
  RenderSystem::RenderSystem()
  {

  }

  void RenderSystem::update(entt::registry &registry, Renderer& renderer, const Camera& camera)
  {
    renderer.init("world");

    for (int i = 0; i < camera.size.w; ++i)
    {
      for (int j = 0; j < camera.size.h; ++j)
      {
        renderer.batch("world", std::make_shared<Sprite>("spritesheet-tileset", 1), (camera.position.x + i)*32, (camera.position.y + j)*32, camera.position.z);
        /* const auto& tile = m_world.get(camera.position.x + i, camera.position.y + j, camera.position.z); */
        /* console.at(i, j).ch = tile.symbol; */
        /* console.at(i, j).fg = { tile.r, tile.g, tile.b, tile.a }; */
      }
    }

    auto view = registry.view<const Position, const Visibility>();
    view.each([&renderer, camera](const auto &position, const auto &visibility) { 
      const auto position_x = static_cast<int>(std::round(position.x - camera.position.x)) * 32;
      const auto position_y = static_cast<int>(std::round(position.y - camera.position.y)) * 32;

      renderer.batch("world", visibility.sprite, position_x, position_y, 1.);

      /* if (!console.in_bounds({position_x, position_y})) */
      /* { */
      /*   return; */
      /* } */
      /* console.at(position.x - camera.position.x, position.y - camera.position.y).ch = visibility.ch; */
      /* console.at(position.x - camera.position.x, position.y - camera.position.y).fg = { visibility.r, visibility.g, visibility.b, 255 }; */
    });
    renderer.finalize("world");
  }
}

