#include "./world_generation.hpp"

#include <spdlog/spdlog.h>

#include "imgui.h"
#include "world/chunk_manager.hpp"
#include "world/generators/island_data.hpp"

namespace
{
dl::IslandNoiseParams original_island_params{};
}

namespace dl
{
WorldGeneration::WorldGeneration(ChunkManager& chunk_manager) : m_chunk_manager(chunk_manager)
{
#ifdef DL_BUILD_DEBUG_TOOLS
  original_island_params = m_chunk_manager.island_params;
#endif
}

void WorldGeneration::update()
{
  if (!open)
  {
    return;
  }

#ifdef DL_BUILD_DEBUG_TOOLS
  if (ImGui::Begin("World Generation", &open, ImGuiWindowFlags_NoFocusOnAppearing))
  {
    ImGui::SeparatorText("General");
    ImGui::DragFloat("Frequency", &m_chunk_manager.island_params.frequency, 0.001f);
    ImGui::DragFloat("Tier Land", &m_chunk_manager.island_params.tier_land, 0.001f);
    ImGui::Checkbox("Display mask", &m_chunk_manager.island_params.display_mask);

    ImGui::SeparatorText("First Layer");
    ImGui::DragInt("Octaves", &m_chunk_manager.island_params.layer_1_octaves, 1);
    ImGui::DragFloat("Lacunarity", &m_chunk_manager.island_params.layer_1_lacunarity, 0.01f);
    ImGui::DragFloat("Gain", &m_chunk_manager.island_params.layer_1_gain, 0.01f);
    ImGui::DragFloat("Weighted Strength", &m_chunk_manager.island_params.layer_1_weighted_strength, 0.01f);
    ImGui::DragFloat("Terrace Multiplier", &m_chunk_manager.island_params.layer_1_terrace_multiplier, 0.01f);
    ImGui::DragScalar("Remap from X", ImGuiDataType_Double, &m_chunk_manager.island_params.layer_1_remap_from.x, 0.1);
    ImGui::DragScalar("Remap from Y", ImGuiDataType_Double, &m_chunk_manager.island_params.layer_1_remap_from.y, 0.1);
    ImGui::DragScalar("Remap to X", ImGuiDataType_Double, &m_chunk_manager.island_params.layer_1_remap_to.x, 0.1);
    ImGui::DragScalar("Remap to Y", ImGuiDataType_Double, &m_chunk_manager.island_params.layer_1_remap_to.y, 0.1);

    ImGui::SeparatorText("Second Layer");
    ImGui::DragInt("Seed Offset", &m_chunk_manager.island_params.layer_2_seed_offset, 1);
    ImGui::DragInt("Octaves2", &m_chunk_manager.island_params.layer_2_octave_count, 1);
    ImGui::DragFloat("Lacunarity2", &m_chunk_manager.island_params.layer_2_lacunarity, 0.01f);
    ImGui::DragFloat("Gain2", &m_chunk_manager.island_params.layer_2_gain, 0.01f);
    ImGui::DragFloat("Weighted Strength2", &m_chunk_manager.island_params.layer_2_weighted_strength, 0.01f);
    ImGui::DragFloat("Fade", &m_chunk_manager.island_params.layer_2_fade, 0.01f);
    ImGui::DragFloat("Smooth RHS", &m_chunk_manager.island_params.layer_2_smooth_rhs, 0.01f);
    ImGui::DragFloat("Smoothness", &m_chunk_manager.island_params.layer_2_smoothness, 0.01f);

    if (ImGui::Button("Regenerate"))
    {
      m_chunk_manager.regenerate_chunks();
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset"))
    {
      m_chunk_manager.island_params = original_island_params;
      m_chunk_manager.regenerate_chunks();
    }

    ImGui::SameLine();

    if (ImGui::Button("Print Values"))
    {
      spdlog::info("World generation values:");
      printf("Frequency: %f\n", m_chunk_manager.island_params.frequency);
      printf("Tier Land: %f\n", m_chunk_manager.island_params.tier_land);
      printf("Display mask: %d\n", m_chunk_manager.island_params.display_mask);
      printf("Octaves: %d\n", m_chunk_manager.island_params.layer_1_octaves);
      printf("Lacunarity: %f\n", m_chunk_manager.island_params.layer_1_lacunarity);
      printf("Gain: %f\n", m_chunk_manager.island_params.layer_1_gain);
      printf("Weighted Strength: %f\n", m_chunk_manager.island_params.layer_1_weighted_strength);
      printf("Terrace Multiplier: %f\n", m_chunk_manager.island_params.layer_1_terrace_multiplier);
      printf("Remap from X: %f\n", m_chunk_manager.island_params.layer_1_remap_from.x);
      printf("Remap from Y: %f\n", m_chunk_manager.island_params.layer_1_remap_from.y);
      printf("Remap to X: %f\n", m_chunk_manager.island_params.layer_1_remap_to.x);
      printf("Remap to Y: %f\n", m_chunk_manager.island_params.layer_1_remap_to.y);
      printf("Seed Offset: %d\n", m_chunk_manager.island_params.layer_2_seed_offset);
      printf("Octaves2: %d\n", m_chunk_manager.island_params.layer_2_octave_count);
      printf("Lacunarity2: %f\n", m_chunk_manager.island_params.layer_2_lacunarity);
      printf("Gain2: %f\n", m_chunk_manager.island_params.layer_2_gain);
      printf("Weighted Strength2: %f\n", m_chunk_manager.island_params.layer_2_weighted_strength);
      printf("Fade: %f\n", m_chunk_manager.island_params.layer_2_fade);
      printf("Smooth RHS: %f\n", m_chunk_manager.island_params.layer_2_smooth_rhs);
      printf("Smoothness: %f\n", m_chunk_manager.island_params.layer_2_smoothness);
    }
  }
  ImGui::End();
#endif
}
}  // namespace dl
