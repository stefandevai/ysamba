#include "./renderer.hpp"

#include "core/asset_manager.hpp"
#include "graphics/camera.hpp"

namespace dl::v2
{
Renderer::Renderer(AssetManager& asset_manager) : m_asset_manager(asset_manager) {}

void Renderer::init() {}

void Renderer::render(const Camera& camera) {}

void Renderer::clear_color(const uint8_t r, const uint8_t g, const uint8_t b, const float a) {}
}  // namespace dl::v2
