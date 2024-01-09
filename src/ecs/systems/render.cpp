#include "./render.hpp"

#include <spdlog/spdlog.h>
#include <tgmath.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/batch.hpp"
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
  Texture* world_texture_ptr = m_world_texture.get();

  for (const auto& tile_data : m_world.tile_data)
  {
    const auto& frame_data = m_world_texture->id_to_frame(tile_data.first, frame_data_type::tile);

    glm::vec2 size{};
    size.x = m_world_texture->get_frame_width();
    size.y = m_world_texture->get_frame_height();
    auto uv_coordinates = m_world_texture->get_frame_coords(frame_data.frame);
    m_tiles.insert(
        {tile_data.first, TileRenderData{world_texture_ptr, &frame_data, std::move(size), std::move(uv_coordinates)}});
  }
}

void RenderSystem::render(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  if (m_batch == nullptr)
  {
    m_batch = m_renderer.get_batch("world"_hs);
  }

  m_render_tiles(camera);

  {
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
          visibility.sprite->frame_angle = frame_data.angle;
        }
      }

      const auto sprite_size = visibility.sprite->get_size();

      const auto position_x = std::round(position.x) * sprite_size.x;
      const auto position_y = std::round(position.y) * sprite_size.y;
      const auto position_z = std::round(position.z) * sprite_size.y;

      m_batch->emplace(
          visibility.sprite.get(), position_x, position_y, position_z + visibility.layer_z * m_z_index_increment);
    }
  }

  {
    const auto& tile_size = m_world.get_tile_size();
    auto quad_view = registry.view<const Position, const Rectangle>();

    for (auto entity : quad_view)
    {
      const auto& position = registry.get<Position>(entity);
      auto& rectangle = registry.get<Rectangle>(entity);

      const auto position_z = std::round(position.z) * tile_size.y;
      const auto position_x = std::round(position.x) * tile_size.x;
      const auto position_y = std::round(position.y) * tile_size.y;

      m_batch->quad(&rectangle.quad, position_x, position_y, position_z + rectangle.z_index * m_z_index_increment);
    }

    auto text_view = registry.view<const Text, const Position>();

    for (auto entity : text_view)
    {
      const auto& position = registry.get<Position>(entity);
      auto& text = registry.get<Text>(entity);

      m_batch->text(text, position.x, position.y, position.z + 3);
    }
  }
}

void RenderSystem::m_render_tiles(const Camera& camera)
{
  const auto& camera_size = camera.get_size_in_tiles();
  const auto& tile_size = m_world.get_tile_size();
  const auto& camera_position = camera.get_position_in_tiles();
  const auto& chunk_size = m_world.chunk_manager.chunk_size;

  {
    const auto first_chunk_position =
        m_world.chunk_manager.world_to_chunk(Vector3i{camera_position.x, camera_position.y, 0});

    // Loop through chunks
    for (int j = first_chunk_position.y; j < camera_position.y + camera_size.y; j += chunk_size.y)
    {
      for (int i = first_chunk_position.x; i < camera_position.x + camera_size.x; i += chunk_size.y)
      {
        const auto& chunk = m_world.chunk_manager.at(i, j, 0);

        if (chunk.tiles.height_map.empty())
        {
          continue;
        }

        int lower_bound_j = 0;
        int lower_bound_i = 0;
        int upper_bound_j = chunk_size.y;
        int upper_bound_i = chunk_size.x;

        if (j < camera_position.y)
        {
          lower_bound_j = camera_position.y - j;
        }
        if (i < camera_position.x)
        {
          lower_bound_i = camera_position.x - i;
        }
        if ((j + chunk_size.y) - (camera_position.y + camera_size.y) > 0)
        {
          upper_bound_j = chunk_size.y - ((j + chunk_size.y) - (camera_position.y + camera_size.y));
        }
        if ((i + chunk_size.x) - (camera_position.x + camera_size.x) > 0)
        {
          upper_bound_i = chunk_size.x - ((i + chunk_size.x) - (camera_position.x + camera_size.x));
        }

        for (int local_j = lower_bound_j; local_j < upper_bound_j; ++local_j)
        {
          for (int local_i = lower_bound_i; local_i < upper_bound_i; ++local_i)
          {
            const auto height = chunk.tiles.height_map[local_i + local_j * chunk_size.x];

            for (int z = height; z >= 0; --z)
            {
              if (!chunk.tiles.has_flags(DL_CELL_FLAG_VISIBLE, local_i, local_j, z))
              {
                continue;
              }

              const auto& terrain_id = chunk.tiles.id_at(local_i, local_j, z);

              if (terrain_id == 0)
              {
                continue;
              }

              m_render_tile(chunk, terrain_id, tile_size, local_i, local_j, z);
            }
          }
        }
      }
    }
  }

  {
    // Render visible tiles with y coordinate to the bottom of the camera frustum
    // Each z level translates to tile_size.y increase in height in clip space,
    // therefore the highest y that can appear on the screen is
    // camera_position.y + camera_size.y + world_size.z.
    // Loop through chunks below the camera bottom to render tiles in other chunks
    // that are high enough to be viewed from the current camera position.
    const auto first_chunk_position =
        m_world.chunk_manager.world_to_chunk(Vector3i{camera_position.x, camera_position.y + camera_size.y, 0});
    const auto last_chunk_position = m_world.chunk_manager.world_to_chunk(
        Vector3i{camera_position.x, camera_position.y + camera_size.y + chunk_size.z, 0});
    for (int j = first_chunk_position.y; j <= last_chunk_position.y; j += chunk_size.y)
    {
      for (int i = first_chunk_position.x; i < camera_position.x + camera_size.x; i += chunk_size.y)
      {
        const auto& chunk = m_world.chunk_manager.at(i, j, 0);

        if (chunk.tiles.height_map.empty())
        {
          continue;
        }

        int lower_bound_j = 0;
        int upper_bound_j = chunk_size.z;

        if (j < camera_position.y + camera_size.y)
        {
          lower_bound_j = (camera_position.y + camera_size.y) - j;
          upper_bound_j = std::min(chunk_size.z + lower_bound_j, chunk_size.y);
        }

        int lower_bound_i = 0;
        int upper_bound_i = chunk_size.x;

        if (i < camera_position.x)
        {
          lower_bound_i = camera_position.x - i;
        }
        if ((i + chunk_size.x) - (camera_position.x + camera_size.x) > 0)
        {
          upper_bound_i = chunk_size.x - ((i + chunk_size.x) - (camera_position.x + camera_size.x));
        }

        for (int local_j = lower_bound_j; local_j < upper_bound_j; ++local_j)
        {
          const auto offset = (j + local_j) - (camera_position.y + camera_size.y);

          for (int local_i = lower_bound_i; local_i < upper_bound_i; ++local_i)
          {
            const auto height = chunk.tiles.height_map[local_i + local_j * chunk_size.x];

            if (height < offset)
            {
              continue;
            }

            for (int z = height; z >= 0; --z)
            {
              if (!chunk.tiles.has_flags(DL_CELL_FLAG_VISIBLE, local_i, local_j, z))
              {
                continue;
              }

              const auto& terrain_id = chunk.tiles.id_at(local_i, local_j, z);

              if (terrain_id == 0)
              {
                continue;
              }

              m_render_tile(chunk, terrain_id, tile_size, local_i, local_j, z);
            }
          }
        }
      }
    }
  }
}

void RenderSystem::m_render_tile(const Chunk& chunk,
                                 const uint32_t tile_id,
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

  const int world_x = chunk.position.x + x;
  const int world_y = chunk.position.y + y;
  const int world_z = chunk.position.z + z;

  const auto& tile = m_tiles.at(tile_id);

  if (tile.frame_data->tile_type == TileType::Single)
  {
    m_batch->tile(
        tile, world_x * tile_size.x, world_y * tile_size.y, world_z * tile_size.y + z_index * m_z_index_increment);

    if (y < 31 && chunk.tiles.is_bottom_empty(x, y, z))
    {
      const auto& bottom_tile = m_tiles.at(tile.frame_data->front_face_id);
      m_batch->tile(bottom_tile, world_x * tile_size.x, world_y * tile_size.y, (world_z - 1) * tile_size.y);
    }
  }
  // else if (tile.frame_data->tile_type == TileType::Multiple)
  // {
  //   // TODO: Check pattern directly on chunk to avoid another lookup
  //   if (!m_world.has_pattern(tile.frame_data->pattern,
  //                            Vector2i{(int)tile.frame_data->pattern_width, (int)tile.frame_data->pattern_height},
  //                            Vector3i{x, y, z}))
  //   {
  //     return;
  //   }
  //
  //   // TODO: Add multi sprite pool
  //   auto multi_sprite =
  //       MultiSprite{m_world_texture_id, tile.frame_data->frame, tile.frame_data->width, tile.frame_data->height};
  //   multi_sprite.texture = m_world_texture;
  //   multi_sprite.frame_angle = tile.frame_data->angle;
  //
  //   m_renderer.batch("world"_hs,
  //                    &multi_sprite,
  //                    (x - tile.frame_data->anchor_x) * tile_size.x,
  //                    (y - tile.frame_data->anchor_y) * tile_size.y,
  //                    z * tile_size.y + z_index);
  // }
}

}  // namespace dl
