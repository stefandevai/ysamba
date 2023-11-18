#include <tgmath.h>
#include <iostream>
#include "./render.hpp"
#include "../../world/world.hpp"
#include "../../world/camera.hpp"
#include "../components/position.hpp"
#include "../components/visibility.hpp"
#include "../../graphics/renderer.hpp"

namespace dl
{
  RenderSystem::RenderSystem(World& world)
    : m_world(world)
  {

  }

  void RenderSystem::update(entt::registry &registry, Renderer& renderer, const Camera& camera)
  {
    renderer.init("world");

    for (int i = 0; i < camera.size.w; ++i)
    {
      for (int j = 0; j < camera.size.h; ++j)
      {
        const auto& tile = m_world.get(i + camera.position.x, j + camera.position.y, camera.position.z);
        const auto& sprite = std::make_shared<Sprite>("spritesheet-tileset", tile.id);

        if (sprite->texture == nullptr)
        {
          sprite->texture = renderer.get_texture(sprite->resource_id);
        }

        const auto sprite_size = sprite->get_size();
        renderer.batch("world", sprite, i*sprite_size.x, j*sprite_size.y, camera.position.z);
      }
    }

    auto view = registry.view<const Position, const Visibility>();
    view.each([&renderer, camera](const auto &position, const auto &visibility) { 
      if (visibility.sprite->texture == nullptr)
      {
        visibility.sprite->texture = renderer.get_texture(visibility.sprite->resource_id);
      }

      const auto sprite_size = visibility.sprite->get_size();
      const auto position_x = static_cast<int>(std::round(position.x - camera.position.x)) * sprite_size.x;
      const auto position_y = static_cast<int>(std::round(position.y - camera.position.y)) * sprite_size.y;

      renderer.batch("world", visibility.sprite, position_x, position_y, 1.);
    });
    renderer.finalize("world");
  }
}

