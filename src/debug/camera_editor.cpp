#include "./camera_editor.hpp"

#include <string>

#include "graphics/camera.hpp"
#include "imgui.h"

namespace dl
{
CameraEditor::CameraEditor(Camera& camera) : m_camera(camera) {}

void CameraEditor::update()
{
  ImGui::Begin("Camera Editor", &m_open);
  ImGui::SeparatorText("Virtual Position");
  ImGui::DragScalar("vx", ImGuiDataType_Double, &m_camera.position.x, 1.0);
  ImGui::DragScalar("vy", ImGuiDataType_Double, &m_camera.position.y, 1.0);
  ImGui::DragScalar("vz", ImGuiDataType_Double, &m_camera.position.z, 1.0);

  ImGui::SeparatorText("Position");
  ImGui::DragScalar("x", ImGuiDataType_Double, &m_camera.m_position.x, 1.0);
  ImGui::DragScalar("y", ImGuiDataType_Double, &m_camera.m_position.y, 1.0);
  ImGui::DragScalar("z", ImGuiDataType_Double, &m_camera.m_position.z, 1.0);

  ImGui::SeparatorText("Rotation");
  ImGui::DragFloat("yaw", &m_camera.yaw, 1.0);
  ImGui::DragFloat("pitch", &m_camera.pitch, 1.0);

  ImGui::SeparatorText("Front");
  ImGui::DragFloat("fx", &m_camera.m_front.x, 1.0);
  ImGui::DragFloat("fy", &m_camera.m_front.y, 1.0);
  ImGui::DragFloat("fz", &m_camera.m_front.z, 1.0);

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
  ImGui::End();
}
}  // namespace dl
