#include "./world_creation.hpp"

#include <fmt/chrono.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <climits>
#include <entt/core/hashed_string.hpp>

#include "config.hpp"
#include "core/game_context.hpp"
#include "core/json.hpp"
#include "core/maths/vector.hpp"
#include "core/random.hpp"
#include "core/scene_manager.hpp"
#include "core/serialization.hpp"
#include "core/utils.hpp"
#include "graphics/color.hpp"
#include "graphics/renderer/texture.hpp"
#include "ui/compositions/world_creation_panel.hpp"
#include "world/generators/island_generator.hpp"
#include "world/metadata.hpp"

namespace dl
{
WorldCreation::WorldCreation(GameContext& game_context) : Scene("world_creation", game_context) {}

void WorldCreation::load()
{
  world_size.x = config::world_creation::world_width;
  world_size.y = config::world_creation::world_height;
  world_size.z = config::world_creation::world_depth;

  m_panel = m_ui_manager.emplace<ui::WorldCreationPanel>();
  m_panel->on_save([this]() { save(); });

  m_generate_map();
  m_generate_world();
  m_create_map_representation();
  m_has_loaded = true;
}

void WorldCreation::update()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  if (m_input_manager.is_context("world_creation"_hs))
  {
    m_update_input();
  }

  m_camera.update(m_game_context.clock->delta);
  m_ui_manager.update();

  if (m_scene_state == SceneState::Pop)
  {
    m_game_context.scene_manager->pop_scene();
  }
}

void WorldCreation::render()
{
  if (!has_loaded())
  {
    return;
  }

  assert(m_texture != nullptr);

  m_renderer.ui_pass.batch.texture(*m_texture, 16, 16, 0);
  m_ui_manager.render();
  m_renderer.render(m_camera);
}

bool WorldCreation::m_update_input()
{
  using namespace entt::literals;

  bool will_quit = false;

  if (m_input_manager.poll_action("quit"_hs))
  {
    m_scene_state = SceneState::Pop;
  }
  else if (m_input_manager.poll_action("generate_world"_hs))
  {
    m_generate_map();
    m_create_map_representation();
  }
  else if (m_input_manager.poll_action("save"_hs))
  {
    save();
  }
  else if (m_input_manager.poll_action("display_seed"_hs))
  {
    spdlog::info("SEED: {}", m_seed);
  }

  return will_quit;
}

void WorldCreation::save()
{
  bool is_valid = m_panel->validate();

  if (!is_valid)
  {
    spdlog::debug("World parameters are invalid");
    return;
  }

  const auto now = std::chrono::system_clock::now();
  const auto now_seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);

  WorldMetadata metadata{};
  metadata.id = utils::generate_id();
  metadata.name = m_panel->get_name();
  metadata.seed = m_seed;
  metadata.world_size = world_size;
  metadata.created_at = now_seconds;
  metadata.updated_at = now_seconds;

  serialization::save_world_metadata(metadata);
  serialization::save_world(m_world, metadata);

  spdlog::debug("World saved: {}", metadata.name);
  spdlog::debug("Seed: {}", metadata.seed);
  spdlog::debug("Id: {}", metadata.id);

  m_scene_state = SceneState::Pop;
}

void WorldCreation::m_generate_map()
{
  // Reload scene file
  JSON json{m_scene_path};

  bool use_random_seed = true;

  if (json.object.contains("random_seed"))
  {
    use_random_seed = json.object["random_seed"].get<bool>();
  }

  if (!use_random_seed && json.object.contains("seed"))
  {
    m_seed = json.object["seed"].get<int>();
  }
  else
  {
    m_seed = random::get_integer(1, INT_MAX);
  }

  auto generator = IslandGenerator(world_size);
  m_height_map.clear();
  m_height_map.reserve(world_size.x * world_size.y);
  generator.generate(m_seed);
  m_height_map = std::move(generator.raw_height_map);
}

void WorldCreation::m_generate_world() { m_world.generate_societies(); }

void WorldCreation::m_create_map_representation()
{
  std::vector<unsigned char> pixel_data((world_size.x * world_size.y) * 4);
  const int z_levels = world_size.z;

  for (auto i = 0; i < world_size.x; ++i)
  {
    for (auto j = 0; j < world_size.y; ++j)
    {
      // const auto map_value = m_height_map[j * world_size.x + i] * 0.5f + 0.5f;
      const auto map_value = m_height_map[j * world_size.x + i];
      const int z = static_cast<int>(map_value * z_levels);
      const uint8_t value = z * 255 / z_levels;
      // const uint8_t value = map_value * 255;

      pixel_data[j * world_size.x * 4 + i * 4] = 0;
      pixel_data[j * world_size.x * 4 + i * 4 + 1] = value;
      pixel_data[j * world_size.x * 4 + i * 4 + 2] = 50;
      pixel_data[j * world_size.x * 4 + i * 4 + 3] = 255;
    }
  }

  m_texture = std::make_unique<Texture>(
      m_game_context.display->wgpu_context.device, pixel_data.data(), Vector2i{world_size.x, world_size.y});

  m_renderer.ui_pass.batch.clear_textures();
}

}  // namespace dl
