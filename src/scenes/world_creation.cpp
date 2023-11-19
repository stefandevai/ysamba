#include "./world_creation.hpp"

#include <SDL.h>
#include <spdlog/spdlog.h>

#include <cereal/archives/binary.hpp>
#include <climits>
#include <fstream>
#include <random>

#include "../graphics/camera.hpp"
#include "../graphics/color.hpp"
#include "../graphics/quad.hpp"
#include "../graphics/renderer.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/texture.hpp"

namespace dl
{
WorldCreation::WorldCreation(const std::string& scene_key, ViewCamera& camera) : Scene(scene_key, camera) {}

void WorldCreation::load()
{
  Scene::load();

  m_world_sprite = std::make_shared<Sprite>();

  const auto seed = m_lua.get_optional_variable<int>("seed");

  if (seed)
  {
    m_generate_map(seed.value());
  }
  else
  {
    m_generate_map();
  }
  m_should_update_world_representation = true;
  m_has_loaded = true;
}

void WorldCreation::update(const uint32_t delta, SetSceneFunction set_scene)
{
  (void)delta;

  if (!has_loaded())
  {
    return;
  }

  if (m_input_manager->poll_action("quit"))
  {
    set_scene("home_menu", m_camera);
  }
  else if (m_input_manager->poll_action("generate_world"))
  {
    m_generate_map();
    m_should_update_world_representation = true;
  }
  else if (m_input_manager->poll_action("reload_world"))
  {
    m_generate_map(m_seed);
    m_should_update_world_representation = true;
  }
  else if (m_input_manager->poll_action("save_world"))
  {
    save_world("./world.dl");
  }
  else if (m_input_manager->poll_action("load_world"))
  {
    load_world("./world.dl");
    m_should_update_world_representation = true;
  }
  else if (m_input_manager->poll_action("display_seed"))
  {
    spdlog::info("SEED: {}", m_world.get_seed());
  }
}

void WorldCreation::render(Renderer& renderer)
{
  if (!has_loaded())
  {
    return;
  }

  /* if (!renderer.has_layer("color")) */
  /* { */
  /*   renderer.add_layer("color", "quad", Renderer::LayerType::QUAD); */
  /* } */

  if (m_should_update_world_representation)
  {
    m_create_world_representation();
    m_should_update_world_representation = false;
  }

  /* renderer.init("color"); */
  /* for (const auto& [position, quad] : m_world_representation) */
  /* { */
  /*   renderer.batch("color", quad, position.x, position.y, 0); */
  /* } */
  /* renderer.finalize("color"); */
  renderer.init("gui");
  renderer.batch("gui", m_world_sprite, 0, 0, 0);
  renderer.finalize("gui");
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

void WorldCreation::save_world(const std::string& file_path)
{
  std::ofstream output_stream(file_path);
  cereal::BinaryOutputArchive archive(output_stream);
  archive(m_world);
}

void WorldCreation::load_world(const std::string& file_path)
{
  std::ifstream input_stream(file_path);
  cereal::BinaryInputArchive archive(input_stream);
  archive(m_world);
  m_has_loaded = true;
}

void WorldCreation::m_generate_map(const int seed)
{
  const int map_width = m_lua.get_variable<int>("map_width");
  const int map_height = m_lua.get_variable<int>("map_height");

  if (seed != 0)
  {
    m_seed = seed;
  }
  else
  {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<int> dist(1, INT_MAX);

    m_seed = dist(rng);
  }

  m_world.generate(map_width, map_height, m_seed);
}

void WorldCreation::m_create_world_representation()
{
  const auto tilemap_size = m_world.get_tilemap_size(0);

  std::vector<unsigned char> pixel_data((tilemap_size.w * tilemap_size.h) * 4);

  const auto water_color = Color("#3772ebff");
  const auto terrain_color = Color("#37c737ff");
  const auto sand_color = Color("#edcb89ff");
  const auto wall_color = Color("#636b5cff");
  const auto default_color = Color("#000000ff");

  for (auto i = 0; i < tilemap_size.w; ++i)
  {
    for (auto j = 0; j < tilemap_size.h; ++j)
    {
      const auto tile = m_world.get(i, j, 0);
      /* auto quad = std::make_shared<Quad>(); */

      switch (tile.id)
      {
      case 1:
        pixel_data[j * tilemap_size.w * 4 + i * 4] = water_color.rgba_color.r;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 1] = water_color.rgba_color.g;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 2] = water_color.rgba_color.b;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 3] = water_color.rgba_color.a;
        /* quad->color = Color("#3772ebff"); */
        break;
      case 2:
        pixel_data[j * tilemap_size.w * 4 + i * 4] = terrain_color.rgba_color.r;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 1] = terrain_color.rgba_color.g;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 2] = terrain_color.rgba_color.b;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 3] = terrain_color.rgba_color.a;
        /* quad->color = Color("#37c737ff"); */
        break;
      case 3:
        pixel_data[j * tilemap_size.w * 4 + i * 4] = sand_color.rgba_color.r;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 1] = sand_color.rgba_color.g;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 2] = sand_color.rgba_color.b;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 3] = sand_color.rgba_color.a;
        /* quad->color = Color("#edcb89ff"); */
        break;
      case 4:
        pixel_data[j * tilemap_size.w * 4 + i * 4] = wall_color.rgba_color.r;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 1] = wall_color.rgba_color.g;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 2] = wall_color.rgba_color.b;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 3] = wall_color.rgba_color.a;
        /* quad->color = Color("#636b5cff"); */
        break;
      default:
        pixel_data[j * tilemap_size.w * 4 + i * 4] = default_color.rgba_color.r;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 1] = default_color.rgba_color.g;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 2] = default_color.rgba_color.b;
        pixel_data[j * tilemap_size.w * 4 + i * 4 + 3] = default_color.rgba_color.a;
        /* quad->color = Color("#000000ff"); */
        break;
      }
      /* quad->w = tile_width; */
      /* quad->h = tile_height; */

      /* const auto position = glm::vec2(i * tile_width, j * tile_height); */

      /* m_world_representation.push_back(std::make_pair(position, quad)); */
    }
  }

  /* std::vector<unsigned char> pixel_data{ */
  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */

  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */

  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */

  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */
  /*   255, 0, 255, 255, */
  /* }; */

  /* const auto texture = std::make_shared<Texture>(pixel_data, 4, 4); */
  const auto texture = std::make_shared<Texture>(pixel_data, tilemap_size.w, tilemap_size.h);
  m_world_sprite->texture = texture;
}
}  // namespace dl
