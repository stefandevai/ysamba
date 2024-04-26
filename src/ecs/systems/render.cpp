#include "./render.hpp"

#include <spdlog/spdlog.h>
#include <tgmath.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include "constants.hpp"
#include "core/asset_manager.hpp"
#include "core/game_context.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/sprite.hpp"
#include "graphics/camera.hpp"
#include "graphics/frame_data_types.hpp"
#include "graphics/quad.hpp"
#include "graphics/renderer/batch.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/text.hpp"
#include "world/world.hpp"

namespace dl
{
RenderSystem::RenderSystem(GameContext& game_context, World& world)
    : m_game_context(game_context),
      m_renderer(*m_game_context.renderer),
      m_batch(m_renderer.main_pass.batch),
      m_world(world),
      m_tile_size(world.get_tile_size())
{
  assert(m_game_context.asset_manager != nullptr);

  const auto& world_spritesheet = m_game_context.asset_manager->get<Spritesheet>(m_world.get_spritesheet_id());

  for (const auto& tile_data : m_world.tile_data)
  {
    const auto& frame_data = world_spritesheet->id_to_frame(tile_data.first, frame_data_type::tile);
    const auto& frame_size = world_spritesheet->get_frame_size();
    glm::vec2 size{frame_size.x * frame_data.width, frame_size.y * frame_data.height};
    m_tiles.insert({tile_data.first, Tile{world_spritesheet, &frame_data, std::move(size)}});
  }

  assert(m_game_context.registry != nullptr);
  m_game_context.registry->on_construct<Sprite>().connect<&RenderSystem::m_create_sprite>(this);
}

void RenderSystem::render(entt::registry& registry, const Camera& camera)
{
  using namespace entt::literals;

  m_render_map_tiles(camera);

  {
    auto items_view = registry.view<const Position, const Sprite>();

    for (auto entity : items_view)
    {
      const auto& position = registry.get<Position>(entity);
      auto& render_data = registry.get<Sprite>(entity);

      const auto position_x = std::round(position.x) * m_tile_size.x - render_data.anchor.x;
      const auto position_y
          = std::round(position.y) * m_tile_size.y - render_data.anchor.y + render_data.layer_z * m_z_index_increment;
      const auto position_z = std::round(position.z) * m_tile_size.y + render_data.layer_z * m_z_index_increment;

      assert(render_data.spritesheet != nullptr && "Sprite Texture not found");
      assert(render_data.frame_data != nullptr && "Sprite Frame data not found");

      m_batch.sprite(render_data, position_x, position_y, position_z, render_data.frame_data->default_face);
    }
  }

  {
    auto quad_view = registry.view<const Position, const Quad>();

    for (auto entity : quad_view)
    {
      const auto& position = registry.get<Position>(entity);
      auto& quad = registry.get<Quad>(entity);

      const auto position_z = std::round(position.z) * m_tile_size.y;
      const auto position_x = std::round(position.x) * m_tile_size.x;
      const auto position_y = std::round(position.y) * m_tile_size.y;

      m_batch.quad(quad,
                   position_x,
                   position_y + quad.z_index * m_z_index_increment,
                   position_z + quad.z_index * m_z_index_increment);
    }

    auto text_view = registry.view<const Text, const Position>();

    for (auto entity : text_view)
    {
      const auto& position = registry.get<Position>(entity);
      auto& text = registry.get<Text>(entity);

      m_batch.text(text, position.x, position.y, position.z + 3);
    }
  }
}

void RenderSystem::m_render_map_tiles(const Camera& camera)
{
  const auto& camera_position = camera.get_position_in_tiles();
  const auto& camera_size = camera.get_size_in_tiles();

  // Padding added to the right side to handle certain HDPI screen inconsistencies
  const int padding = 1;

  {
    const auto first_chunk_position
        = m_world.chunk_manager.world_to_chunk(Vector3i{camera_position.x, camera_position.y, 0});

    // Loop through chunks
    for (int j = first_chunk_position.y; j <= camera_position.y + camera_size.y; j += world::chunk_size.y)
    {
      for (int i = first_chunk_position.x; i <= camera_position.x + camera_size.x; i += world::chunk_size.x)
      {
        const auto& chunk = m_world.chunk_manager.at(i, j, 0);

        if (chunk.tiles.height_map.empty())
        {
          continue;
        }

        int lower_bound_j = 0;
        int lower_bound_i = 0;
        int upper_bound_j = world::chunk_size.y;
        int upper_bound_i = world::chunk_size.x;

        if (j < camera_position.y)
        {
          lower_bound_j = camera_position.y - j;
        }
        if (i < camera_position.x)
        {
          lower_bound_i = camera_position.x - i;
        }
        if ((j + world::chunk_size.y) - (camera_position.y + camera_size.y) > 0)
        {
          upper_bound_j
              = world::chunk_size.y - ((j + world::chunk_size.y) - (camera_position.y + camera_size.y + padding));
        }
        if ((i + world::chunk_size.x) - (camera_position.x + camera_size.x) > 0)
        {
          upper_bound_i
              = world::chunk_size.x - ((i + world::chunk_size.x) - (camera_position.x + camera_size.x + padding));
        }

        for (int local_j = lower_bound_j; local_j < upper_bound_j; ++local_j)
        {
          for (int local_i = lower_bound_i; local_i < upper_bound_i; ++local_i)
          {
            const auto height = chunk.tiles.height_map[local_i + local_j * world::chunk_size.x];

            for (int z = height; z >= 0; --z)
            {
              if (!chunk.tiles.has_flags(DL_CELL_FLAG_TOP_FACE_VISIBLE, local_i, local_j, z)
                  && !chunk.tiles.has_flags(DL_CELL_FLAG_FRONT_FACE_VISIBLE, local_i, local_j, z))
              {
                continue;
              }

              const auto& cell = chunk.tiles.cell_at(local_i, local_j, z);
              const Vector3i local_position{local_i, local_j, z};

              if (chunk.tiles.has_flags(DL_CELL_FLAG_TOP_FACE_VISIBLE, local_i, local_j, z))
              {
                m_render_map_tile(chunk, cell.top_face, local_position);
                m_render_map_tile(chunk, cell.top_face_decoration, local_position, 1);
              }
              if (chunk.tiles.has_flags(DL_CELL_FLAG_FRONT_FACE_VISIBLE, local_i, local_j, z))
              {
                m_render_map_tile(chunk, cell.front_face, local_position);
                m_render_map_tile(chunk, cell.front_face_decoration, local_position, 1);
              }
            }
          }
        }
      }
    }
  }

  {
    // Render visible tiles with y coordinate to the bottom of the camera frustum
    // Each z level translates to m_tile_size.y increase in height in clip space,
    // therefore the highest y that can appear on the screen is
    // camera_position.y + camera_size.y + world_size.z.
    // Loop through chunks below the camera bottom to render tiles in other chunks
    // that are high enough to be viewed from the current camera position.
    const auto first_chunk_position
        = m_world.chunk_manager.world_to_chunk(Vector3i{camera_position.x, camera_position.y + camera_size.y, 0});
    const auto last_chunk_position = m_world.chunk_manager.world_to_chunk(
        Vector3i{camera_position.x, camera_position.y + camera_size.y + world::chunk_size.z, 0});
    for (int j = first_chunk_position.y; j <= last_chunk_position.y; j += world::chunk_size.y)
    {
      for (int i = first_chunk_position.x; i < camera_position.x + camera_size.x; i += world::chunk_size.y)
      {
        const auto& chunk = m_world.chunk_manager.at(i, j, 0);

        if (chunk.tiles.height_map.empty())
        {
          continue;
        }

        int lower_bound_j = 0;
        int upper_bound_j = world::chunk_size.z;

        if (j < camera_position.y + camera_size.y)
        {
          lower_bound_j = (camera_position.y + camera_size.y) - j;
          upper_bound_j = std::min(world::chunk_size.z + lower_bound_j, world::chunk_size.y);
        }

        int lower_bound_i = 0;
        int upper_bound_i = world::chunk_size.x;

        if (i < camera_position.x)
        {
          lower_bound_i = camera_position.x - i;
        }
        if ((i + world::chunk_size.x) - (camera_position.x + camera_size.x) > 0)
        {
          upper_bound_i
              = world::chunk_size.x - ((i + world::chunk_size.x) - (camera_position.x + camera_size.x + padding));
        }

        for (int local_j = lower_bound_j; local_j < upper_bound_j; ++local_j)
        {
          const auto offset = (j + local_j) - (camera_position.y + camera_size.y);

          for (int local_i = lower_bound_i; local_i < upper_bound_i; ++local_i)
          {
            const auto height = chunk.tiles.height_map[local_i + local_j * world::chunk_size.x];

            if (height < offset || height >= world::chunk_size.z)
            {
              continue;
            }

            for (int z = height; z >= 0; --z)
            {
              if (!chunk.tiles.has_flags(DL_CELL_FLAG_TOP_FACE_VISIBLE, local_i, local_j, z)
                  && !chunk.tiles.has_flags(DL_CELL_FLAG_FRONT_FACE_VISIBLE, local_i, local_j, z))
              {
                continue;
              }

              const auto& cell = chunk.tiles.cell_at(local_i, local_j, z);
              const Vector3i local_position{local_i, local_j, z};

              if (chunk.tiles.has_flags(DL_CELL_FLAG_TOP_FACE_VISIBLE, local_i, local_j, z))
              {
                m_render_map_tile(chunk, cell.top_face, local_position);
                m_render_map_tile(chunk, cell.top_face_decoration, local_position, 1);
              }
              if (chunk.tiles.has_flags(DL_CELL_FLAG_FRONT_FACE_VISIBLE, local_i, local_j, z))
              {
                m_render_map_tile(chunk, cell.front_face, local_position);
                m_render_map_tile(chunk, cell.front_face_decoration, local_position, 1);
              }
            }
          }
        }
      }
    }
  }
}

void RenderSystem::m_render_map_tile(const Chunk& chunk,
                                     const uint32_t tile_id,
                                     const Vector3i& position,
                                     const int z_index)
{
  if (tile_id <= 0)
  {
    return;
  }

  const auto& tile = m_tiles.at(tile_id);
  const Vector3i world_position = chunk.position + position;

  // TODO: Add anchor and size to single type sprites so that we don't have to branch here
  if (tile.frame_data->sprite_type == SpriteType::Single)
  {
    m_batch.tile(tile,
                 world_position.x * m_tile_size.x,
                 world_position.y * m_tile_size.y + z_index * m_z_index_increment,
                 world_position.z * m_tile_size.y + z_index * m_z_index_increment,
                 tile.frame_data->default_face);

    // // TODO: Add neighbour chunk references to each chunk to be able to check tiles after the chunk bounds
    // if (chunk.tiles.has_flags(DL_CELL_FLAG_FRONT_FACE_VISIBLE, position.x, position.y, position.z))
    // {
    //   m_batch.tile(tile,
    //                world_position.x * m_tile_size.x,
    //                world_position.y * m_tile_size.y,
    //                (world_position.z - 1) * m_tile_size.y,
    //                DL_RENDER_FACE_FRONT);
    // }
  }
  else if (tile.frame_data->sprite_type == SpriteType::Multiple)
  {
    m_batch.tile(tile,
                 (world_position.x - tile.frame_data->anchor_x) * m_tile_size.x,
                 (world_position.y - tile.frame_data->anchor_y) * m_tile_size.y,
                 world_position.z * m_tile_size.y + z_index * m_z_index_increment,
                 tile.frame_data->default_face);
  }
}

void RenderSystem::m_create_sprite(entt::registry& registry, entt::entity entity)
{
  auto& sprite_data = registry.get<Sprite>(entity);

  assert(sprite_data.resource_id != 0 && "Resource ID not found while creating sprite");

  // Load spritesheet and render data
  const auto spritesheet = m_game_context.asset_manager->get<Spritesheet>(sprite_data.resource_id);
  sprite_data.spritesheet = spritesheet;

  assert(spritesheet->has_metadata && "Spritesheet metadata not found");

  sprite_data.load_from_spritesheet();
}

}  // namespace dl
