#pragma once

#include <map>

#include "./components/component.hpp"

namespace dl
{
class Batch;
}  // namespace dl

namespace dl::ui
{
class UIManager
{
 public:
  UIManager();
  void load(std::shared_ptr<Batch> batch);

  uint32_t add_component(const std::shared_ptr<UIComponent>& component);
  void remove_component(const uint32_t id);

  void update();
  void render();

 private:
  std::map<uint32_t, std::shared_ptr<UIComponent>> m_components;
  std::shared_ptr<Batch> m_batch = nullptr;
  /* ShaderLoader m_gui_shader{"./data/shaders/gui"}; */
  /* ShaderLoader m_text_shader{"./data/shaders/text"}; */
  /* Batch2D m_sprite_batch{}; */
  /* BatchQuad m_quad_batch{}; */

  static uint32_t m_identifier() noexcept
  {
    static uint32_t value = 0;
    return value++;
  }
};

}  // namespace dl::ui
