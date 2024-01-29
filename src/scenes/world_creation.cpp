#include "./world_creation.hpp"

#include <spdlog/spdlog.h>

#include <climits>
#include <entt/core/hashed_string.hpp>

#include "core/game_context.hpp"
#include "core/maths/vector.hpp"
#include "core/random.hpp"
#include "core/scene_manager.hpp"
#include "core/serialization.hpp"
#include "graphics/color.hpp"
#include "graphics/renderer.hpp"
#include "graphics/texture.hpp"
#include "world/generators/island_generator.hpp"

namespace dl
{
WorldCreation::WorldCreation(GameContext& game_context) : Scene("world_creation", game_context)
{
  m_renderer.add_batch(&m_batch);
}

void WorldCreation::load()
{
  Scene::load();

  world_size.x = m_json.object["world_width"].get<int>();
  world_size.y = m_json.object["world_height"].get<int>();
  world_size.z = m_json.object["world_depth"].get<int>();

  m_generate_map();
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

  const auto delta = m_game_context.clock->delta;

  m_camera.update(delta);

  if (m_input_manager.poll_action("quit"_hs))
  {
    m_game_context.scene_manager->pop_scene();
  }
  else if (m_input_manager.poll_action("generate_world"_hs))
  {
    m_generate_map();
    m_create_map_representation();
  }
  else if (m_input_manager.poll_action("reload_world"_hs))
  {
    m_generate_map();
    m_create_map_representation();
  }
  else if (m_input_manager.poll_action("save_world"_hs))
  {
    save_world();
  }
  else if (m_input_manager.poll_action("load_world"_hs))
  {
    load_world();
    m_create_map_representation();
  }
  else if (m_input_manager.poll_action("display_seed"_hs))
  {
    spdlog::info("SEED: {}", m_world.get_seed());
  }
}

void WorldCreation::render()
{
  if (!has_loaded())
  {
    return;
  }

  m_batch.push_matrix(m_camera.view_matrix);
  m_batch.emplace(&m_world_sprite, 0, 0, 0);
  m_renderer.render(m_camera);
  m_batch.pop_matrix();
}

void WorldCreation::save_world() { serialization::save_world(m_world); }

void WorldCreation::load_world()
{
  serialization::load_world(m_world);
  m_has_loaded = true;
}

void WorldCreation::m_generate_map()
{
  // Reload scene file
  m_json.load(m_scene_path);

  const auto& json = m_json.object;
  bool use_random_seed = true;
  int seed = 0;

  if (json.contains("random_seed"))
  {
    use_random_seed = json["random_seed"].get<bool>();
  }

  if (!use_random_seed && json.contains("seed"))
  {
    seed = json["seed"].get<int>();
  }
  else
  {
    seed = random::get_integer(1, INT_MAX);
  }

  auto generator = IslandGenerator(world_size);
  m_height_map.clear();
  m_height_map.reserve(world_size.x * world_size.y);
  generator.generate(seed);
  m_height_map = std::move(generator.raw_height_map);
}

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

  const auto texture = std::make_shared<Texture>(pixel_data, world_size.x, world_size.y);
  m_world_sprite.texture = texture;
  m_batch.clear_textures();
}

}  // namespace dl
