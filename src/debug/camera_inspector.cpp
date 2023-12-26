#include "./camera_inspector.hpp"

#include <string>

#include "graphics/camera.hpp"
#include "imgui.h"

namespace dl
{
CameraInspector::CameraInspector(Camera& camera) : m_camera(camera) {}

void CameraInspector::update()
{
  if (!open)
  {
    return;
  }

  ImGui::Begin("Camera Inspector", &open);
  ImGui::SeparatorText("Virtual Position");
  ImGui::DragScalar("vx", ImGuiDataType_Double, &m_camera.position.x, 1.0);
  ImGui::DragScalar("vy", ImGuiDataType_Double, &m_camera.position.y, 1.0);
  ImGui::DragScalar("vz", ImGuiDataType_Double, &m_camera.position.z, 1.0);

  ImGui::SeparatorText("Position");
  ImGui::DragFloat("x", &m_camera.m_position.x, 1.0f);
  ImGui::DragFloat("y", &m_camera.m_position.y, 1.0f);
  ImGui::DragFloat("z", &m_camera.m_position.z, 1.0f);

  ImGui::SeparatorText("Zoom");
  ImGui::Text("%f", m_camera.zoom);
  if (ImGui::Button("+"))
  {
    m_camera.zoom_in();
  }
  ImGui::SameLine();
  if (ImGui::Button("-"))
  {
    m_camera.zoom_out();
  }

  static float yaw = m_camera.yaw;
  static float pitch = m_camera.pitch;

  ImGui::SeparatorText("Rotation");
  ImGui::DragFloat("yaw", &yaw, 1.0);
  ImGui::DragFloat("pitch", &pitch, 1.0);

  if (yaw != m_camera.yaw)
  {
    m_camera.set_yaw(yaw);
  }
  if (pitch != m_camera.pitch)
  {
    m_camera.set_pitch(pitch);
  }

  ImGui::SeparatorText("Other");
  ImGui::DragFloat("Frustrum left", &m_camera.m_frustrum_left, 1.0f);
  ImGui::DragFloat("Frustrum right", &m_camera.m_frustrum_right, 1.0f);
  ImGui::DragFloat("Frustrum bottom", &m_camera.m_frustrum_bottom, 1.0f);
  ImGui::DragFloat("Frustrum top", &m_camera.m_frustrum_top, 1.0f);
  ImGui::DragFloat("Near", &m_camera.m_near, 1.0f);
  ImGui::DragFloat("Far", &m_camera.m_far, 1.0f);
  ImGui::Checkbox("Resize View Matrix", &m_camera.m_resize_view_matrix);

  ImGui::SeparatorText("Info");
  const auto& position = m_camera.get_position();
  const auto& tile_position = m_camera.get_position_in_tiles();
  ImGui::Text("get_position: (%s, %s, %s)",
              std::to_string(position.x).c_str(),
              std::to_string(position.y).c_str(),
              std::to_string(position.z).c_str());
  ImGui::Text("get_position_in_tiles: (%s, %s)",
              std::to_string(tile_position.x).c_str(),
              std::to_string(tile_position.y).c_str());
  ImGui::Text("m_size: (%s, %s)", std::to_string(m_camera.m_size.x).c_str(), std::to_string(m_camera.m_size.y).c_str());
  ImGui::Text("m_size_in_tiles: (%s, %s)",
              std::to_string(m_camera.m_size_in_tiles.x).c_str(),
              std::to_string(m_camera.m_size_in_tiles.y).c_str());
  ImGui::Text("m_grid_size: (%s, %s)",
              std::to_string(m_camera.m_grid_size.x).c_str(),
              std::to_string(m_camera.m_grid_size.y).c_str());
  ImGui::End();
}
}  // namespace dl
