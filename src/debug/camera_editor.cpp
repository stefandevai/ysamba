#include "./camera_editor.hpp"

#include "graphics/camera.hpp"
#include "imgui.h"

namespace dl
{
CameraEditor::CameraEditor(Camera& camera) : m_camera(camera) {}

void CameraEditor::update()
{
  ImGui::Begin("Camera Editor", &m_open);
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
  ImGui::End();
}
}  // namespace dl
