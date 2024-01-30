#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <unordered_map>
#include <variant>

#include "./json.hpp"
#include "graphics/font.hpp"
#include "graphics/shader_program.hpp"
#include "graphics/texture.hpp"

namespace dl
{
using Asset = std::variant<std::unique_ptr<ShaderProgram>, std::unique_ptr<Texture>, std::unique_ptr<Font>>;

enum class AssetType
{
  None,
  Texture,
  Shader,
  TextureAtlas,
  Font,
};

class AssetManager
{
 public:
  AssetManager() = default;

  void load_assets(const std::filesystem::path& filepath);

  template <typename T, typename... Args>
  void add(uint32_t id, Args&&... args)
  {
    std::unique_ptr<T> asset = std::make_unique<T>(std::forward<Args>(args)...);
    m_assets.emplace(id, std::move(asset));
  }

  template <typename T>
  static T* get(uint32_t id)
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
      value->load();
    }

    return value.get();
  }

 private:
  JSON m_json{};
  static std::unordered_map<uint32_t, Asset> m_assets;
  static const std::unordered_map<std::string, AssetType> m_asset_types;
};
}  // namespace dl
