#include "./general_info.hpp"

#include "core/game_context.hpp"
#include "imgui.h"

namespace dl
{
GeneralInfo::GeneralInfo(GameContext& context) : m_game_context(context) {}

void GeneralInfo::update()
{
  ImGui::Begin("General Info", &m_open);
  ImGui::Text(("FPS: " + std::to_string(1.0 / m_game_context.clock->delta)).c_str());
  ImGui::Text(("MS: " + std::to_string(m_game_context.clock->delta)).c_str());
  ImGui::End();
}
}  // namespace dl
