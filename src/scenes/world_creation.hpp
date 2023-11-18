#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <entt/entity/registry.hpp>
#include "./scene.hpp"
#include "../core/input_manager.hpp"
#include "../world/world.hpp"

namespace dl
{
  class Renderer;
  struct Quad;

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
      using WorldRepresentation = std::vector<std::pair<glm::vec2, std::shared_ptr<Quad>>>;
      World m_world;
      WorldRepresentation m_world_representation;
      std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
      int m_seed = 110;
      bool m_should_update_world_representation = false;

      void m_generate_map(const int seed = 0);
      void m_create_world_representation();
  };
}
