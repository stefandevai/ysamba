#pragma once

#include <spdlog/spdlog.h>
#include <webgpu/wgpu.h>

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "audio/sound_buffer.hpp"
#include "audio/sound_stream_buffer.hpp"
#include "graphics/display.hpp"
#include "graphics/font.hpp"
#include "graphics/renderer/spritesheet.hpp"
#include "graphics/renderer/texture.hpp"
#include "graphics/renderer/texture_atlas.hpp"

namespace dl
{
using Asset = std::variant<std::unique_ptr<Texture>,
                           std::unique_ptr<TextureAtlas>,
                           std::unique_ptr<Spritesheet>,
                           std::unique_ptr<Font>,
                           std::unique_ptr<audio::SoundBuffer>,
                           std::unique_ptr<audio::SoundStreamBuffer>>;

struct AssetLoader
{
  AssetLoader(const WGPUDevice& device) : m_device(device) {}

  void operator()(const std::unique_ptr<Texture>& texture) { texture->load(m_device); }

  void operator()(const std::unique_ptr<TextureAtlas>& atlas) { atlas->load(m_device); }

  void operator()(const std::unique_ptr<Spritesheet>& spritesheet) { spritesheet->load(m_device); }

  void operator()(const std::unique_ptr<Font>& font) { font->load(m_device); }

  void operator()(const std::unique_ptr<audio::SoundBuffer>& buffer) { buffer->load(); }

  void operator()(const std::unique_ptr<audio::SoundStreamBuffer>& buffer) { buffer->load(); }

 private:
  const WGPUDevice& m_device;
};

enum class AssetType
{
  None,
  Texture,
  TextureAtlas,
  Spritesheet,
  Font,
  Music,
  SoundEffect,
};

class AssetManager
{
 public:
  AssetManager(const Display& display) : m_display(display) {}

  // Loads assets given an JSON definition file
  void load_assets(const std::filesystem::path& filepath);

  // Gets an asset by its underlying type
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

  // Returns a vector of handles containing ids mapped to types for all loaded assets.
  // This allows easy iteration through all assets.
  const std::vector<std::pair<uint32_t, AssetType>>& get_ids() const { return m_asset_ids; }

 private:
  // Hash map containing the actual assets referenced by its key
  std::unordered_map<uint32_t, Asset> m_assets{};

  // Strings to AssetType enum in order to parse atypes from JSON
  const std::unordered_map<std::string, AssetType> m_asset_types = {
      {"texture", AssetType::Texture},
      {"texture_atlas", AssetType::TextureAtlas},
      {"spritesheet", AssetType::Spritesheet},
      {"font", AssetType::Font},
      {"music", AssetType::Music},
      {"sound_effect", AssetType::SoundEffect},
  };

  // Contains ids mapped to types for all loaded assets. Allow easy iteration through all assets
  std::vector<std::pair<uint32_t, AssetType>> m_asset_ids{};

  const Display& m_display;

  template <typename T, typename... Args>
  void m_add(uint32_t id, Args&&... args)
  {
    std::unique_ptr<T> asset = std::make_unique<T>(std::forward<Args>(args)...);
    m_assets.emplace(id, std::move(asset));
  }
};
}  // namespace dl
