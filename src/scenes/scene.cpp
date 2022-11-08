#include "./scene.hpp"

#include <SDL.h>

namespace dl
{
  Scene::Scene(const std::string& scene_path)
    : m_scene_path(scene_path)
  {

  }

  Scene::~Scene()
  {

  }

  void Scene::load()
  {
    m_lua.load(m_scene_path);
  }

  void Scene::screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename)
  {
    (void)console;

    int w, h;
    auto renderer = context.get_sdl_renderer();
    SDL_GetRendererOutputSize(renderer, &w, &h);
    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surface->pixels, surface->pitch);
    SDL_SaveBMP(surface, (filename + ".bmp").c_str());
    SDL_FreeSurface(surface);
  }
}

