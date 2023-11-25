#include "./scene.hpp"

#include <SDL.h>

#include "core/input_manager.hpp"
#include "graphics/camera.hpp"

namespace dl
{
Scene::Scene(const std::string& scene_key, Camera& camera)
    : m_scene_key(scene_key), m_scene_dir("scenes/" + scene_key + "/"), m_camera(camera)
{
}

Scene::~Scene() {}

void Scene::load()
{
  auto input_manager = InputManager::get_instance();
  m_lua.load(m_scene_dir / "main.lua");
  input_manager->set_context(m_scene_key);
}

/* void Scene::screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename) */
/* { */
/*   (void)console; */

/*   int w, h; */
/*   auto renderer = context.get_sdl_renderer(); */
/*   SDL_GetRendererOutputSize(renderer, &w, &h); */
/*   SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0); */
/*   SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surface->pixels, surface->pitch); */
/*   SDL_SaveBMP(surface, (filename + ".bmp").c_str()); */
/*   SDL_FreeSurface(surface); */
/* } */
}  // namespace dl
