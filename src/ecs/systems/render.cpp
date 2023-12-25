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
RenderSystem::RenderSystem(Renderer& renderer, World& world)
    : m_renderer(renderer),
      m_world(world),
      m_world_texture_id(m_world.get_texture_id()),
      m_world_texture(m_renderer.get_texture(m_world.get_texture_id()))
{
}

void RenderSystem::render(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  const auto& camera_size = camera.get_size_in_tiles();
  const auto& tile_size = camera.get_tile_size();
  const auto& camera_position = camera.get_position_in_tiles();
  const auto& world_size = m_world.tiles.size;

  for (int j = -m_frustum_tile_padding; j < camera_size.y + m_frustum_tile_padding; ++j)
  {
    for (int i = -m_frustum_tile_padding; i < camera_size.x + m_frustum_tile_padding; ++i)
    {
      const auto index_x = i + camera_position.x;
      const auto index_y = j + camera_position.y;

      if (index_x < 0 || index_y < 0 || index_x >= world_size.x || index_y >= world_size.y)
      {
        continue;
      }

      const auto z = m_world.tiles.height_map[index_y * world_size.x + index_x];

      const auto& terrain = m_world.get_terrain(index_x, index_y, z);
      m_render_tile(terrain.id, camera_position, tile_size, i, j, z);

      const auto& over_terrain = m_world.get_over_terrain(index_x, index_y, z);
      m_render_tile(over_terrain.id, camera_position, tile_size, i, j, z, 1);
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
      visibility.sprite->texture = m_renderer.get_texture(visibility.sprite->resource_id);

      // Set specific frame according to the texture data loaded in a separated json file.
      // This allows flexibility by separating the texture frames from game ids.
      if (visibility.frame_id > 0 && !visibility.frame_type.empty())
      {
        const auto& frame_data = visibility.sprite->texture->id_to_frame(visibility.frame_id, visibility.frame_type);
        visibility.sprite->set_frame(frame_data.frame);
      }
    }

    const auto sprite_size = visibility.sprite->get_size();
    m_batch<Sprite>(position, visibility.sprite.get(), Vector2i{sprite_size.x, sprite_size.y}, visibility.layer_z);
  }

  auto quad_view = registry.view<const Position, const Rectangle>();

  for (auto entity : quad_view)
  {
    const auto& position = registry.get<Position>(entity);
    auto& rectangle = registry.get<Rectangle>(entity);

    m_batch<Quad>(position, &rectangle.quad, tile_size, rectangle.z_index);
  }

  auto text_view = registry.view<const Text, const Position>();

  for (auto entity : text_view)
  {
    const auto& position = registry.get<Position>(entity);
    auto& text = registry.get<Text>(entity);

    m_renderer.batch("world"_hs, text, position.x, position.y, position.z + 3);
  }
}

template <typename T>
void RenderSystem::m_batch(const Position& position, T* renderable, const Vector2i& size, const int z_index)
{
  using namespace entt::literals;

  const auto position_z = std::round(position.z) * size.y;
  const auto position_x = std::round(position.x) * size.x;
  const auto position_y = std::round(position.y) * size.y;

  m_renderer.batch("world"_hs, renderable, position_x, position_y, position_z + z_index * m_z_index_increment);
}

void RenderSystem::m_render_tile(const uint32_t tile_id,
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

  const auto& frame_data = m_world_texture->id_to_frame(tile_id, frame_data_type::tile);

  const int transformed_x = x + camera_position.x;
  const int transformed_y = y + camera_position.y;

  if (frame_data.tile_type == "single")
  {
    // TODO: Add sprite pool
    auto sprite = Sprite{m_world_texture_id, 0};
    sprite.texture = m_world_texture;
    sprite.set_frame(frame_data.frame);

    m_renderer.batch("world"_hs,
                     &sprite,
                     transformed_x * tile_size.x,
                     transformed_y * tile_size.y,
                     z * tile_size.y + z_index * m_z_index_increment);

    /* if (perspective == TopDown45) */
    /* { */
    /*   const auto& bottom_tile = m_world.get_terrain(x + camera_position.x, y + camera_position.y + 1, z); */

    /*   if (bottom_tile.id == 0) */
    /*   { */
    /*     auto bottom_sprite = Sprite{m_world_texture_id, 0}; */
    /*     bottom_sprite.texture = m_world_texture; */
    /*     bottom_sprite.set_frame(168); */
    /*     m_renderer.batch("world"_hs, */
    /*                      &bottom_sprite, */
    /*                      transformed_x * tile_size.x, */
    /*                      (y - z + 1 + camera_position.y) * tile_size.y, */
    /*                      (z - 1) * tile_size.y + z_index); */
    /*   } */
    /* } */
  }
  else if (frame_data.tile_type == "multiple")
  {
    if (!m_world.has_pattern(frame_data.pattern,
                             Vector2i{(int)frame_data.pattern_width, (int)frame_data.pattern_height},
                             Vector3i{transformed_x, transformed_y, z}))
    {
      return;
    }

    // TODO: Add multi sprite pool
    auto multi_sprite = MultiSprite{m_world_texture_id, frame_data.frame, frame_data.width, frame_data.height};
    multi_sprite.texture = m_world_texture;

    m_renderer.batch("world"_hs,
                     &multi_sprite,
                     (transformed_x - frame_data.anchor_x) * tile_size.x,
                     (transformed_y - frame_data.anchor_y) * tile_size.y,
                     z * tile_size.y + z_index);
  }
}

}  // namespace dl
