#include "game.hpp"

#include "./clock.hpp"
#include "./file_manager.hpp"

namespace dl
{
  Game::Game() { }

  void Game::load()
  {
    try
    {
      const auto width = m_json.object["width"];
      const auto height = m_json.object["height"];
      const auto title = m_json.object["title"];

      m_display.load(width, height, title);
      m_camera.set_frustrum (0.0f, width, height, 0.0f);
      m_renderer.add_layer("world", "world");
      m_renderer.add_layer("quad", "quad", Renderer::LayerType::Quad);
      m_renderer.add_layer("gui", "gui", Renderer::LayerType::Sprite, true);
      m_renderer.add_layer("text", "text", Renderer::LayerType::Sprite, true);
    }
    catch (const std::exception& exc)
    {
      std::cerr << exc.what() << "\n";
      throw;
    }
  }

  void Game::run()
  {
    try
    {
      dl::Clock clock{};

      while (!m_input_manager->should_quit())
      {
        clock.tick();

        const auto dt = clock.delta / 1000.f;
        m_camera.move(-20.f*dt, 0.f, 0.f);

        m_scene_manager.update(clock.delta);
        m_input_manager->update();

        m_display.clear();
        m_scene_manager.render(m_renderer);
        m_renderer.render(m_camera);
        m_display.render();
      }
    }
    catch (const std::exception& exc)
    {
      std::cerr << exc.what() << "\n";
      throw;
    }
  }
}
