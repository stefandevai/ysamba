#include "./render.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <tgmath.h>

#include <entt/entity/registry.hpp>

#include "ecs/components/position.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/text.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/camera.hpp"
#include "graphics/frame_data_types.hpp"
#include "graphics/renderer.hpp"
#include "graphics/text.hpp"
#include "world/world.hpp"

namespace dl
{
RenderSystem::RenderSystem(World& world) : m_world(world), m_world_texture_id(m_world.get_texture_id()) {}

void RenderSystem::render(entt::registry& registry, Renderer& renderer, const Camera& camera)
{
  const auto& camera_size = camera.get_size_in_tiles();
  const auto& tile_size = camera.get_tile_size();
  const auto& camera_position = camera.get_position_in_tiles();

  for (int i = -m_frustum_tile_padding; i < camera_size.x + m_frustum_tile_padding; ++i)
  {
    for (int j = -m_frustum_tile_padding; j < camera_size.y + m_frustum_tile_padding; ++j)
    {
      const auto index_x = i + camera_position.x;
      const auto index_y = j + camera_position.y;
      const auto& tile = m_world.get(index_x, index_y, 0.0);
      const auto& sprite = std::make_shared<Sprite>(m_world_texture_id, 0);

      if (sprite->texture == nullptr)
      {
        const auto& world_texture = renderer.get_texture(m_world_texture_id);
        const auto& frame = world_texture->id_to_frame(tile.id, frame_data_type::tile);
        sprite->texture = world_texture;
        sprite->set_frame(frame);
      }

      renderer.batch("world",
                     sprite,
                     i * tile_size.x + camera_position.x * tile_size.x,
                     j * tile_size.y + camera_position.y * tile_size.y,
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

    renderer.batch("world", visibility.sprite, position_x, position_y, visibility.layer_z);
  }

  glDisable(GL_DEPTH_TEST);

  auto quad_view = registry.view<const Position, const Rectangle>();

  for (auto entity : quad_view)
  {
    const auto& position = registry.get<Position>(entity);
    const auto& rectangle = registry.get<Rectangle>(entity);

    renderer.batch("ui", rectangle.quad, position.x, position.y, 10);
  }

  auto text_view = registry.view<const Text, const Position>();
  for (auto entity : text_view)
  {
    const auto& position = registry.get<Position>(entity);
    auto& text = registry.get<Text>(entity);

    renderer.batch("ui", text, position.x, position.y, 3);
  }
}
}  // namespace dl
