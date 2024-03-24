#pragma once

#include <spdlog/spdlog.h>
#include <webgpu/wgpu.h>

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <variant>

#include "graphics/display.hpp"
#include "graphics/font.hpp"
#include "graphics/renderer/texture.hpp"

namespace dl
{
using Asset = std::variant<std::unique_ptr<Texture>, std::unique_ptr<Font>>;

struct AssetLoader
{
  AssetLoader(const WGPUDevice& device) : m_device(device) {}

  void operator()(const std::unique_ptr<Texture>& texture) { texture->load(m_device); }

  void operator()(const std::unique_ptr<Font>& font) { font->load(m_device); }

 private:
  const WGPUDevice& m_device;
};

enum class AssetType
{
  None,
  Texture,
  TextureAtlas,
  Font,
};

class AssetManager
{
 public:
  AssetManager(const Display& display) : m_display(display) {}

  void load_assets(const std::filesystem::path& filepath);

  template <typename T, typename... Args>
  void add(uint32_t id, Args&&... args)
  {
    std::unique_ptr<T> asset = std::make_unique<T>(std::forward<Args>(args)...);
    m_assets.emplace(id, std::move(asset));
  }

  template <typename T>
  T* get(uint32_t id)
  {
    if (!m_assets.contains(id))
    {
      printf("Could not find asset with id %d\n", id);
      return nullptr;
    }

    auto& asset = m_assets.at(id);
    auto& value = std::get<std::unique_ptr<T>>(asset);

    if (!value->has_loaded)
    {
      std::visit(AssetLoader{m_display.wgpu_context.device}, asset);
    }

    return value.get();
  }

 private:
  std::unordered_map<uint32_t, Asset> m_assets{};
  const std::unordered_map<std::string, AssetType> m_asset_types = {
      {"texture", AssetType::Texture},
      {"texture_atlas", AssetType::TextureAtlas},
      {"font", AssetType::Font},
  };
  const Display& m_display;
};
}  // namespace dl
