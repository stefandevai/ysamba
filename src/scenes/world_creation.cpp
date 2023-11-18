#include "./world_creation.hpp"

#include <fstream>
#include <random>
#include <climits>
#include <SDL.h>
#include <cereal/archives/binary.hpp>
#include "../graphics/renderer.hpp"
#include "../graphics/quad.hpp"
#include "../graphics/color.hpp"

namespace dl
{
  WorldCreation::WorldCreation(const std::string& scene_key)
    : Scene(scene_key)
  { }

  void WorldCreation::load()
  {
    Scene::load();

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

  void WorldCreation::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    (void)set_scene;

    if (!has_loaded())
    {
      return;
    }

    if (m_input_manager->poll_action("quit"))
    {
      set_scene("home_menu");
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
      std::cout << "SEED: " << m_world.get_seed() << '\n';
    }
  }

  void WorldCreation::render(Renderer& renderer)
  {
    if (!has_loaded())
    {
      return;
    }

    if (!renderer.has_layer("color"))
    {
      renderer.add_layer("color", "quad", Renderer::LayerType::QUAD);
    }

    if (m_should_update_world_representation)
    {
      m_create_world_representation();
      m_should_update_world_representation = false;
    }

    renderer.init("color");
    for (const auto& [position, quad] : m_world_representation)
    {
      renderer.batch("color", quad, position.x, position.y, 0);
    }
    renderer.finalize("color");
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
    cereal::BinaryOutputArchive archive (output_stream);
    archive(m_world);
  }

  void WorldCreation::load_world(const std::string& file_path)
  {
    std::ifstream input_stream(file_path);
    cereal::BinaryInputArchive archive (input_stream);
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

    const auto tile_width = 3; 
    const auto tile_height = 3; 

    const auto representation_w = static_cast<int>(tilemap_size.w / 6);
    const auto representation_h = static_cast<int>(tilemap_size.h / 6);

    for (auto i = 0; i < representation_w; ++i)
    {
      for (auto j = 0; j < representation_h; ++j)
      {
        const auto tile = m_world.get(i * 6, j * 6, 0);
        auto quad = std::make_shared<Quad>();

        switch(tile.id)
        {
          case 1:
            quad->color = Color("#3772ebff");
            break;
          case 2:
            quad->color = Color("#37c737ff");
            break;
          case 3:
            quad->color = Color("#edcb89ff");
            break;
          case 4:
            quad->color = Color("#636b5cff");
            break;
          default:
            quad->color = Color("#000000ff");
            break;
        }
        quad->w = tile_width;
        quad->h = tile_height;

        const auto position = glm::vec2(i * tile_width, j * tile_height);

        m_world_representation.push_back(std::make_pair(position, quad));
      }
    }
  }
}
