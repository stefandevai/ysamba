#include "./world_creation.hpp"

#include <spdlog/spdlog.h>

#include <climits>
#include <entt/core/hashed_string.hpp>

#include "core/game_context.hpp"
#include "core/random.hpp"
#include "core/scene_manager.hpp"
#include "core/serialization.hpp"
#include "graphics/color.hpp"
#include "graphics/renderer.hpp"
#include "graphics/sprite.hpp"
#include "graphics/texture.hpp"
#include "world/generators/map_generator.hpp"

namespace dl
{
WorldCreation::WorldCreation(GameContext& game_context) : Scene("world_creation", game_context)
{
  using namespace entt::literals;

  m_renderer.add_batch("gui"_hs, "default", 2);
}

void WorldCreation::load()
{
  Scene::load();

  m_world_sprite = std::make_unique<Sprite>();

  const auto seed = m_json.object["seed"].get<int>();

  if (seed)
  {
    m_generate_height_map(seed);
  }
  else
  {
    m_generate_height_map();
  }
  m_create_height_map_representation();
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
    m_generate_height_map();
    m_create_height_map_representation();
  }
  else if (m_input_manager.poll_action("reload_world"_hs))
  {
    m_generate_height_map(m_seed);
    m_create_height_map_representation();
  }
  else if (m_input_manager.poll_action("save_world"_hs))
  {
    save_world();
  }
  else if (m_input_manager.poll_action("load_world"_hs))
  {
    load_world();
    m_create_height_map_representation();
  }
  else if (m_input_manager.poll_action("display_seed"_hs))
  {
    spdlog::info("SEED: {}", m_world.get_seed());
  }
}

void WorldCreation::render()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  m_renderer.push_matrix("gui"_hs, m_camera.view_matrix);
  m_renderer.batch("gui"_hs, m_world_sprite.get(), 0, 0, 0);
  m_renderer.render(m_camera);
  m_renderer.pop_matrix("gui"_hs);
}

/* void WorldCreation::screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename) */
/* { */
/*   (void)context; */
/*   (void)console; */

/*   const auto tilemap_size = m_world.get_tilemap_size(0); */
/*   SDL_Surface *surface = SDL_CreateRGBSurface(0, tilemap_size.w, tilemap_size.h, 32, 0, 0, 0, 0); */

/*   for (auto i = 0; i < tilemap_size.w; ++i) */
/*   { */
/*     for (auto j = 0; j < tilemap_size.h; ++j) */
/*     { */
/*       SDL_Rect rect; */
/*       rect.x = i; */
/*       rect.y = j; */
/*       rect.w = 1; */
/*       rect.h = 1; */

/*       const auto tile = m_world.get(i, j, 0); */
/*       const auto color = SDL_MapRGB(surface->format, tile.r, tile.g, tile.b); */

/*       SDL_FillRect(surface, &rect, color); */
/*     } */
/*   } */

/*   SDL_SaveBMP(surface, (filename + ".bmp").c_str()); */

/*   SDL_FreeSurface(surface); */
/* } */

void WorldCreation::save_world() { serialization::save_world(m_world); }

void WorldCreation::load_world()
{
  serialization::load_world(m_world);
  m_has_loaded = true;
}

void WorldCreation::m_generate_map(const int seed)
{
  const int map_width = m_json.object["map_width"].get<int>();
  const int map_height = m_json.object["map_height"].get<int>();

  if (seed != 0)
  {
    m_seed = seed;
  }
  else
  {
    m_seed = random::get_integer(1, INT_MAX);
  }

  m_world.generate(map_width, map_height, 10, m_seed);
}

void WorldCreation::m_create_world_representation()
{
  /* const auto& size = m_world.tiles.size; */

  /* std::vector<unsigned char> pixel_data((size.x * size.y) * 4); */

  /* const auto water_color = Color(0x3772ebff); */
  /* const auto terrain_color = Color(0x37c737ff); */
  /* const auto sand_color = Color(0xedcb89ff); */
  /* const auto wall_color = Color(0x636b5cff); */
  /* const auto default_color = Color(0x000000ff); */

  /* for (auto i = 0; i < size.x; ++i) */
  /* { */
  /*   for (auto j = 0; j < size.y; ++j) */
  /*   { */
  /*     const auto& tile = m_world.get(i, j, 0); */

  /*     switch (tile.id) */
  /*     { */
  /*     case 1: */
  /*       pixel_data[j * size.x * 4 + i * 4] = water_color.rgba_color.r; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 1] = water_color.rgba_color.g; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 2] = water_color.rgba_color.b; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 3] = water_color.rgba_color.a; */
  /*       break; */
  /*     case 2: */
  /*       pixel_data[j * size.x * 4 + i * 4] = terrain_color.rgba_color.r; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 1] = terrain_color.rgba_color.g; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 2] = terrain_color.rgba_color.b; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 3] = terrain_color.rgba_color.a; */
  /*       break; */
  /*     case 3: */
  /*       pixel_data[j * size.x * 4 + i * 4] = sand_color.rgba_color.r; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 1] = sand_color.rgba_color.g; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 2] = sand_color.rgba_color.b; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 3] = sand_color.rgba_color.a; */
  /*       break; */
  /*     case 4: */
  /*       pixel_data[j * size.x * 4 + i * 4] = wall_color.rgba_color.r; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 1] = wall_color.rgba_color.g; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 2] = wall_color.rgba_color.b; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 3] = wall_color.rgba_color.a; */
  /*       break; */
  /*     default: */
  /*       pixel_data[j * size.x * 4 + i * 4] = default_color.rgba_color.r; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 1] = default_color.rgba_color.g; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 2] = default_color.rgba_color.b; */
  /*       pixel_data[j * size.x * 4 + i * 4 + 3] = default_color.rgba_color.a; */
  /*       break; */
  /*     } */
  /*   } */
  /* } */

  /* const auto texture = std::make_shared<Texture>(pixel_data, size.x, size.y); */
  /* m_world_sprite->texture = texture; */
}

const int hm_width = 256;
const int hm_height = 256;

void WorldCreation::m_generate_height_map(const int seed)
{
  (void)seed;

  const auto s = random::get_integer(1, INT_MAX);
  auto generator = MapGenerator(hm_width, hm_width);
  m_height_map.clear();
  m_height_map.reserve(hm_width * hm_height);
  generator.generate(s);
  m_height_map = std::move(generator.raw_height_map);
}

void WorldCreation::m_create_height_map_representation()
{
  std::vector<unsigned char> pixel_data((hm_width * hm_height) * 4);
  const int z_levels = 10;

  for (auto i = 0; i < hm_width; ++i)
  {
    for (auto j = 0; j < hm_height; ++j)
    {
      const auto map_value = m_height_map[j * hm_width + i];
      const int z = static_cast<int>(map_value * z_levels);
      const uint8_t value = z * 255 / z_levels;

      pixel_data[j * hm_width * 4 + i * 4] = 0;
      pixel_data[j * hm_width * 4 + i * 4 + 1] = value;
      pixel_data[j * hm_width * 4 + i * 4 + 2] = 50;
      pixel_data[j * hm_width * 4 + i * 4 + 3] = 255;
    }
  }

  const auto texture = std::make_shared<Texture>(pixel_data, hm_width, hm_height);
  m_world_sprite->texture = texture;
}

}  // namespace dl
