#include "./render.hpp"

#include <spdlog/spdlog.h>
#include <tgmath.h>

#include <entt/entity/registry.hpp>

#include "../../graphics/camera.hpp"
#include "../../graphics/renderer.hpp"
#include "../../world/world.hpp"
#include "../components/position.hpp"
#include "../components/rectangle.hpp"
#include "../components/selectable.hpp"
#include "../components/visibility.hpp"

namespace dl
{
RenderSystem::RenderSystem(World& world) : m_world(world) {}

void RenderSystem::update(entt::registry& registry, Renderer& renderer, const Camera& camera)
{
  // TODO: Cache values so we don't have to calculate them every frame
  const auto& camera_position = camera.get_position();
  const auto& camera_dimensions = camera.get_dimensions();
  const auto& world_texture_id = m_world.get_texture_id();
  const auto& world_texture = renderer.get_texture(world_texture_id);
  const auto tile_width = world_texture->get_frame_width();
  const auto tile_height = world_texture->get_frame_height();
  const auto horizontal_tiles = std::ceil(camera_dimensions.x / tile_width);
  const auto vertical_tiles = std::ceil(camera_dimensions.y / tile_height);
  const int camera_tile_offset_x = (camera_position.x + 1) / tile_width;
  const int camera_tile_offset_y = (camera_position.y + 1) / tile_height;
  const int frustum_tile_padding = 1;

  renderer.init("world");

  for (int i = -frustum_tile_padding; i < horizontal_tiles + frustum_tile_padding; ++i)
  {
    for (int j = -frustum_tile_padding; j < vertical_tiles + frustum_tile_padding; ++j)
    {
      const auto index_x = i + camera_tile_offset_x;
      const auto index_y = j + camera_tile_offset_y;
      const auto& tile = m_world.get(index_x, index_y, 0.0);
      const auto& sprite = std::make_shared<Sprite>(world_texture_id, tile.id);

      if (sprite->texture == nullptr)
      {
        sprite->texture = world_texture;
      }

      renderer.batch("world",
                     sprite,
                     i * tile_width + camera_tile_offset_x * tile_width,
                     j * tile_height + camera_tile_offset_y * tile_height,
                     0.0);
    }
  }

  auto items_view = registry.view<const Position, const Visibility>();

  for (auto entity : items_view)
  {
    const auto& position = registry.get<Position>(entity);
    const auto& visibility = registry.get<Visibility>(entity);

    if (visibility.sprite->texture == nullptr)
    {
      visibility.sprite->texture = renderer.get_texture(visibility.sprite->resource_id);

      // Set specific frame according to the texture data loaded in a separated json file.
      // This allows flexibility by separating the texture frames from game ids.
      if (visibility.frame_id > 0 && !visibility.frame_type.empty())
      {
        const auto frame = visibility.sprite->texture->id_to_frame(visibility.frame_id, visibility.frame_type);
        visibility.sprite->set_frame(frame);
      }
    }

    const auto sprite_size = visibility.sprite->get_size();
    const auto position_x = std::round(position.x) * sprite_size.x;
    const auto position_y = std::round(position.y) * sprite_size.y;

    renderer.batch("world", visibility.sprite, position_x, position_y, 1.);
  }

  renderer.finalize("world");

  renderer.init("quad");

  auto quad_view = registry.view<const Position, const Rectangle>();

  quad_view.each([&renderer](const auto& position, const auto& rectangle) {
    const auto position_x = std::round(position.x) * 32;
    const auto position_y = std::round(position.y) * 32;

    renderer.batch("quad", rectangle.quad, position_x, position_y, 2.);
  });

  renderer.finalize("quad");
}
}  // namespace dl
