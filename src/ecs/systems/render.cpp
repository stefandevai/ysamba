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

              m_render_tile(chunk, terrain_id, tile_size, i + local_i, j + local_j, z);
            }
          }
        }
      }
    }
  }

  // Render visible tiles with y coordinate to the bottom of the camera frustum
  // Each z level translates to tile_size.y increase in height in clip space,
  // therefore the highest y that can appear on the screen is
  // camera_position.y + camera_size.y + world_size.z.
  // Loop through chunks below the camera bottom to render tiles in other chunks
  // that are high enough to be viewed from the current camera position.
  const auto first_chunk_position =
      m_world.chunk_manager.world_to_chunk(Vector3i{camera_position.x, camera_position.y + camera_size.y + 1, 0});
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

            m_render_tile(chunk, terrain_id, tile_size, i + local_i, j + local_j, z);
          }
        }
      }
    }
  }

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
      m_batch<Sprite>(position, visibility.sprite.get(), Vector2i{sprite_size.x, sprite_size.y}, visibility.layer_z);
    }
  }

  {
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

  const auto& frame_data = m_world_texture->id_to_frame(tile_id, frame_data_type::tile);

  if (frame_data.tile_type == "single")
  {
    // TODO: Add sprite pool
    auto sprite = Sprite{m_world_texture_id, 0};
    sprite.texture = m_world_texture;
    sprite.set_frame(frame_data.frame);
    sprite.frame_angle = frame_data.angle;

    m_renderer.batch(
        "world"_hs, &sprite, x * tile_size.x, y * tile_size.y, z * tile_size.y + z_index * m_z_index_increment);

    if (chunk.tiles.is_bottom_empty(x, y, z))
    {
      auto bottom_sprite = Sprite{m_world_texture_id, 0};
      bottom_sprite.texture = m_world_texture;
      bottom_sprite.set_frame(frame_data.front_face_frame);
      bottom_sprite.frame_angle = FrameAngle::Orthogonal;

      m_renderer.batch("world"_hs, &bottom_sprite, x * tile_size.x, y * tile_size.y, (z - 1) * tile_size.y);
    }
  }
  else if (frame_data.tile_type == "multiple")
  {
    // TODO: Check pattern directly on chunk to avoid another lookup
    if (!m_world.has_pattern(frame_data.pattern,
                             Vector2i{(int)frame_data.pattern_width, (int)frame_data.pattern_height},
                             Vector3i{x, y, z}))
    {
      return;
    }

    // TODO: Add multi sprite pool
    auto multi_sprite = MultiSprite{m_world_texture_id, frame_data.frame, frame_data.width, frame_data.height};
    multi_sprite.texture = m_world_texture;
    multi_sprite.frame_angle = frame_data.angle;

    m_renderer.batch("world"_hs,
                     &multi_sprite,
                     (x - frame_data.anchor_x) * tile_size.x,
                     (y - frame_data.anchor_y) * tile_size.y,
                     z * tile_size.y + z_index);
  }
}

}  // namespace dl
