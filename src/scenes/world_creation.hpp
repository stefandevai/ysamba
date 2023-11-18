#pragma once

#include <memory>
#include <entt/entity/registry.hpp>
#include "./scene.hpp"
#include "../core/input_manager.hpp"
#include "../world/world.hpp"
/* #include "../world/camera.hpp" */

namespace dl
{
  class Renderer;

  class WorldCreation : public Scene
  {
    public:
      WorldCreation(const std::string& scene_key);

      void load() override;
      void update(const uint32_t delta, std::function<void(const std::string&)> set_scene) override;
      void render(Renderer& renderer) override;
      /* void screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename) override; */

      void save_world(const std::string& file_path);
      void load_world(const std::string& file_path);

    private:
      World m_world;
      /* Camera m_camera; */
      /* SDL_Surface* m_surface = nullptr; */
      std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
      int m_seed = 110;
      bool m_should_update_world_surface = false;

      void m_generate_map(const int seed = 0);
      /* void m_create_world_surface(tcod::Context& context); */
  };
}
