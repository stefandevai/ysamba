#include "./render_editor.hpp"

#include "ecs/systems/render.hpp"
#include "imgui.h"

namespace dl
{
RenderEditor::RenderEditor(RenderSystem& render) : m_render(render) {}

void RenderEditor::update()
{
  (void)m_render;
  ImGui::Begin("Render Editor", &m_open);
  /* ImGui::SeparatorText("Virtual Position"); */
  /* ImGui::DragInt("Frustum padding", &m_render.m_frustum_tile_padding, 1); */
  /* ImGui::DragScalar("vy", ImGuiDataType_Double, &m_render.position.y, 1.0); */
  /* ImGui::DragScalar("vz", ImGuiDataType_Double, &m_render.position.z, 1.0); */

  /* ImGui::SeparatorText("Position"); */
  /* ImGui::DragScalar("x", ImGuiDataType_Double, &m_render.m_position.x, 1.0); */
  /* ImGui::DragScalar("y", ImGuiDataType_Double, &m_render.m_position.y, 1.0); */
  /* ImGui::DragScalar("z", ImGuiDataType_Double, &m_render.m_position.z, 1.0); */

  /* ImGui::SeparatorText("Rotation"); */
  /* ImGui::DragFloat("yaw", &m_render.yaw, 1.0); */
  /* ImGui::DragFloat("pitch", &m_render.pitch, 1.0); */

  /* ImGui::SeparatorText("Front"); */
  /* ImGui::DragFloat("fx", &m_render.m_front.x, 1.0); */
  /* ImGui::DragFloat("fy", &m_render.m_front.y, 1.0); */
  /* ImGui::DragFloat("fz", &m_render.m_front.z, 1.0); */

  /* ImGui::SeparatorText("Other"); */
  /* ImGui::DragFloat("Frustrum left", &m_render.m_frustrum_left, 1.0f); */
  /* ImGui::DragFloat("Frustrum right", &m_render.m_frustrum_right, 1.0f); */
  /* ImGui::DragFloat("Frustrum bottom", &m_render.m_frustrum_bottom, 1.0f); */
  /* ImGui::DragFloat("Frustrum top", &m_render.m_frustrum_top, 1.0f); */
  /* ImGui::DragFloat("Near", &m_render.m_near, 1.0f); */
  /* ImGui::DragFloat("Far", &m_render.m_far, 1.0f); */
  ImGui::End();
}
}  // namespace dl
