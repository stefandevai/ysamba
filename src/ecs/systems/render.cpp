#include <tgmath.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include "./render.hpp"
#include "../../world/world.hpp"
#include "../../world/camera.hpp"
#include "../components/position.hpp"
#include "../components/visibility.hpp"
#include "../components/selectable.hpp"
#include "../../graphics/renderer.hpp"
#include "../../graphics/camera.hpp"

namespace dl
{
  RenderSystem::RenderSystem(World& world)
    : m_world(world)
  {

  }

  void RenderSystem::update(entt::registry &registry, Renderer& renderer, const ViewCamera& camera)
  {
    // TODO: Cache values so we don't have to calculate them every frame
    const auto& camera_position = camera.get_position();
    const auto& camera_dimensions = camera.get_dimensions();
    const auto& world_texture_id = m_world.get_texture_id();
    const auto& world_texture = renderer.get_texture(world_texture_id);
    const auto tile_width = world_texture->get_frame_width(0);
    const auto tile_height = world_texture->get_frame_height(0);
    const auto horizontal_tiles = std::ceil(camera_dimensions.x / tile_width);
    const auto vertical_tiles = std::ceil(camera_dimensions.y / tile_height);
    const int camera_offset_x = (camera_position.x + 1) / tile_width;
    const int camera_offset_y = (camera_position.y + 1) / tile_height;

    /* spdlog::warn("CAMERA POS: {} {}", camera_position.x, camera_position.y); */

    renderer.init("world");

    for (int i = 0; i < horizontal_tiles; ++i)
    {
      for (int j = 0; j < vertical_tiles; ++j)
      {
        const auto index_x = i + camera_offset_x;
        const auto index_y = j + camera_offset_y;
        const auto& tile = m_world.get(index_x, index_y, 0.0);
        /* const auto& tile = m_world.get(i, j, 0.0); */
        const auto& sprite = std::make_shared<Sprite>(world_texture_id, tile.id);

        if (sprite->texture == nullptr)
        {
          sprite->texture = world_texture;
        }

        renderer.batch("world", sprite, i*tile_width + camera_offset_x*tile_width, j*tile_height + camera_offset_y*tile_height, 0.0);
      }
    }

    /* auto view = registry.view<const Position, const Visibility, const Selectable>(); */
    /* view.each([&renderer, camera_position](const auto &position, const auto &visibility, const auto& selectable) { */ 
    /*   if (visibility.sprite->texture == nullptr) */
    /*   { */
    /*     visibility.sprite->texture = renderer.get_texture(visibility.sprite->resource_id); */
    /*   } */

    /*   if (selectable.selected) */
    /*   { */
    /*     visibility.sprite->set_frame(1); */
    /*   } */

    /*   const auto sprite_size = visibility.sprite->get_size(); */
    /*   const auto position_x = static_cast<int>(std::round(position.x - camera_position.x)) * sprite_size.x; */
    /*   const auto position_y = static_cast<int>(std::round(position.y - camera_position.y)) * sprite_size.y; */

    /*   renderer.batch("world", visibility.sprite, position_x, position_y, 1.); */
    /* }); */
    renderer.finalize("world");
  }
}

