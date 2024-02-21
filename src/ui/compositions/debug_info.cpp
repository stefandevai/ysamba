#include "./debug_info.hpp"

#include <spdlog/spdlog.h>

#include "core/clock.hpp"
#include "graphics/camera.hpp"
#include "ui/components/container.hpp"
#include "ui/components/label.hpp"

namespace dl::ui
{
DebugInfo::DebugInfo(UIContext& context, const Camera& camera) : UIComponent(context), m_camera(camera)
{
  size = {200, 100};
  position = {30, 30, 0};

  m_container = emplace<Container>(size, 0x1b242066);

  m_label = m_container->emplace<Label>();
  m_label->position = {15, 15, 0};
  m_label->wrap = true;

  placement = Placement::Absolute;
}

/* void DebugInfo::set_content(const std::string_view text) { m_label->set_text(text); } */

void DebugInfo::update()
{
  if (m_delay <= 0.0)
  {
    m_fps = "FPS: " + std::to_string(1.0 / m_context.clock->delta);
    m_delay = 0.8;
  }
  else
  {
    m_delay -= m_context.clock->delta;
  }

  const auto mouse_position = m_input_manager.get_mouse_position();
  const auto& camera_position = m_camera.get_position();
  const auto& grid_size = m_camera.get_grid_size();
  const int tile_x = std::floor((mouse_position.x + camera_position.x) / grid_size.x);
  const int tile_y = std::floor((mouse_position.y + camera_position.y) / grid_size.y);

  m_label->set_text(m_fps + "\nTile: (" + std::to_string(tile_x) + ", " + std::to_string(tile_y) + ")" + "\nMouse: ("
                    + std::to_string(mouse_position.x) + ", " + std::to_string(mouse_position.y) + ")");
}
}  // namespace dl::ui
