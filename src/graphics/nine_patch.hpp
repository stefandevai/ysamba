#pragma once

#include <glm/vec2.hpp>

#include "./color.hpp"
#include "./sprite.hpp"
#include "graphics/renderer/texture.hpp"

namespace dl
{
struct NinePatch
{
  // Id of the loaded resource
  uint32_t resource_id{};

  // Top, left, bottom and right coordinates in the texture given in pixels
  uint32_t top{};
  uint32_t left{};
  uint32_t bottom{};
  uint32_t right{};

  // Border value for nine patch calculations
  uint32_t border{};

  // Total size of the nine patch including the border
  glm::vec2 size{};

  // Sprites for each patch
  std::array<Sprite, 8> border_patches{};
  Sprite center_patch{};

  // Actual sprite texture
  const Texture* texture = nullptr;

  // Flag to trigger patche (re)generation
  bool dirty = true;

  // Color overlay
  Color color{0xFFFFFFFF};

  NinePatch() = default;
  NinePatch(const uint32_t resource_id) : resource_id(resource_id) {}
  NinePatch(const uint32_t resource_id,
            const uint32_t top,
            const uint32_t left,
            const uint32_t bottom,
            const uint32_t right,
            const uint32_t border);

  // (Re)generate patches
  void generate_patches();

  // Get top-left, top-right, bottom-right and bottom-left uv coordinates
  std::array<glm::vec2, 4> get_texture_coordinates() const;
};
}  // namespace dl
