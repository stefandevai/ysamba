#include "./render.hpp"

#include <spdlog/spdlog.h>
#include <tgmath.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/camera.hpp"
#include "graphics/frame_data_types.hpp"
#include "graphics/multi_sprite.hpp"
#include "graphics/renderer.hpp"
#include "graphics/text.hpp"
#include "world/world.hpp"

namespace dl
{
enum RenderingType
{
  TopDown90,
  TopDown45,
};

RenderSystem::RenderSystem(World& world) : m_world(world), m_world_texture_id(m_world.get_texture_id()) {}

void RenderSystem::render(entt::registry& registry, Renderer& renderer, const Camera& camera)
{
  using namespace entt::literals;

  const int highest_z = 1;

  const auto& camera_size = camera.get_size_in_tiles();
  const auto& tile_size = camera.get_tile_size();
  const auto& camera_position = camera.get_position_in_tiles();

  for (int i = -m_frustum_tile_padding; i < camera_size.x + m_frustum_tile_padding; ++i)
  {
    for (int j = -m_frustum_tile_padding; j < camera_size.y + m_frustum_tile_padding; ++j)
    {
      const auto index_x = i + camera_position.x;
      const auto index_y = j + camera_position.y;

      for (int z = highest_z; z >= 0; --z)
      {
        const auto& world_tile = m_world.get_all(index_x, index_y, z);
        m_render_tile(renderer, world_tile.terrain.id, camera_position, tile_size, i, j, z);
        m_render_tile(renderer, world_tile.over_terrain.id, camera_position, tile_size, i, j, z, 1);
      }
    }
  }

  auto items_view = registry.view<const Position, const Visibility>();

  for (auto entity : items_view)
  {
    const auto& position = registry.get<Position>(entity);
    auto& visibility = registry.get<Visibility>(entity);

    if (visibility.sprite == nullptr)
    {
      visibility.sprite = std::make_unique<Sprite>(visibility.resource_id, visibility.frame);
    }
    if (visibility.sprite->texture == nullptr)
    {
      visibility.sprite->texture = renderer.get_texture(visibility.sprite->resource_id);

      // Set specific frame according to the texture data loaded in a separated json file.
      // This allows flexibility by separating the texture frames from game ids.
      if (visibility.frame_id > 0 && !visibility.frame_type.empty())
      {
        const auto& frame_data = visibility.sprite->texture->id_to_frame(visibility.frame_id, visibility.frame_type);
        visibility.sprite->set_frame(frame_data.frame);
      }
    }

    const auto sprite_size = visibility.sprite->get_size();
    const auto position_x = std::round(position.x) * sprite_size.x;
    const auto position_y = std::round(position.y) * sprite_size.y;
    const auto position_z = std::round(position.z) * sprite_size.y;

    renderer.batch(
        "world"_hs, visibility.sprite.get(), position_x, position_y - position_z, position_z + visibility.layer_z);
  }

  auto quad_view = registry.view<const Position, const Rectangle>();

  for (auto entity : quad_view)
  {
    const auto& position = registry.get<Position>(entity);
    const auto& rectangle = registry.get<Rectangle>(entity);

    renderer.batch("world"_hs,
                   rectangle.quad.get(),
                   std::round(position.x) * tile_size.x,
                   std::round(position.y) * tile_size.y - position.z * tile_size.y,
                   position.z * tile_size.y);
  }

  auto text_view = registry.view<const Text, const Position>();
  for (auto entity : text_view)
  {
    const auto& position = registry.get<Position>(entity);
    auto& text = registry.get<Text>(entity);

    renderer.batch("world"_hs, text, position.x, position.y, position.z * tile_size.y + 3);
  }
}

void RenderSystem::m_render_tile(Renderer& renderer,
                                 const uint32_t tile_id,
                                 const Vector2i& camera_position,
                                 const Vector2i& tile_size,
                                 const int x,
                                 const int y,
                                 const int z,
                                 const int z_index)
{
  using namespace entt::literals;

  if (tile_id <= 0)
  {
    return;
  }

  const auto& world_texture = renderer.get_texture(m_world_texture_id);
  const auto& frame_data = world_texture->id_to_frame(tile_id, frame_data_type::tile);

  if (frame_data.tile_type == "multiple")
  {
    if (!m_world.has_pattern(frame_data.pattern,
                             Vector2i{(int)frame_data.pattern_width, (int)frame_data.pattern_height},
                             Vector3i{x + camera_position.x, y + camera_position.y, z}))
    {
      return;
    }

    // TODO: Add multi sprite pool
    auto multi_sprite = MultiSprite{m_world_texture_id, frame_data.frame, frame_data.width, frame_data.height};
    multi_sprite.texture = world_texture;

    renderer.batch("world"_hs,
                   &multi_sprite,
                   (x - frame_data.anchor_x) * tile_size.x + camera_position.x * tile_size.x,
                   (y - frame_data.anchor_y) * tile_size.y + camera_position.y * tile_size.y - z * tile_size.y,
                   z * tile_size.y + z_index);
  }
  else if (frame_data.tile_type == "single")
  {
    // TODO: Add sprite pool
    auto sprite = Sprite{m_world_texture_id, 0};
    sprite.texture = world_texture;
    sprite.set_frame(frame_data.frame);

    renderer.batch("world"_hs,
                   &sprite,
                   x * tile_size.x + camera_position.x * tile_size.x,
                   y * tile_size.y + camera_position.y * tile_size.y - z * tile_size.y,
                   /* j * tile_size.y + camera_position.y * tile_size.y, */
                   z * tile_size.y + z_index);
  }
}

}  // namespace dl
