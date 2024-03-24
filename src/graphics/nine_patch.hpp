#pragma once

#include <glm/vec2.hpp>

#include "./color.hpp"
#include "ecs/components/texture_slice.hpp"
#include "graphics/renderer/texture_atlas.hpp"

namespace dl
{
struct NinePatch
{
  // Nine patch ID
  uint32_t id{};

  // Loaded resource ID
  uint32_t resource_id{};

  // Total size of the nine patch including the border
  glm::vec2 size{};

  // Top, left, bottom and right coordinates in the texture given in pixels
  uint32_t top{};
  uint32_t left{};
  uint32_t bottom{};
  uint32_t right{};

  // Border value for nine patch calculations
  uint32_t border{};

  // Sprites for each patch
  std::array<TextureSlice, 8> border_patches{};
  TextureSlice center_patch{};

  // Actual sprite texture
  const TextureAtlas* texture_atlas = nullptr;

  // Flag to trigger patche (re)generation
  bool dirty = true;

  // Color overlay
  Color color{0xFFFFFFFF};

  NinePatch() = default;
  NinePatch(const uint32_t id, const uint32_t resource_id);

  // (Re)generate patches
  void generate_patches();
};
}  // namespace dl
