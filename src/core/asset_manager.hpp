#pragma once

#include <spdlog/spdlog.h>

#include <filesystem>
#include <string>
#include <unordered_map>

#include "./asset.hpp"
#include "./asset_parameters.hpp"
#include "./json.hpp"

namespace dl
{
using AssetPair = std::pair<std::shared_ptr<Asset>, std::unique_ptr<AssetLoader>>;
using AssetMap = std::unordered_map<std::string, AssetPair>;

class AssetManager
{
 public:
  AssetManager(const std::filesystem::path& filepath);

  // Add resource data to the internal map
  template <class T, typename... Args>
  void add(const std::string& name, const std::string& filepath, Args... args);

  // Gets a loaded asset by its name
  template <typename T>
  static std::shared_ptr<T> get(const std::string& id)
  {
    if (!m_assets.contains(id))
    {
      spdlog::warn("There's no asset with ID {}.\n", id);
      return nullptr;
    }

    auto& asset_pair = m_assets.at(id);
    auto& asset_ptr = asset_pair.first;

    if (asset_ptr != nullptr)
    {
      return std::dynamic_pointer_cast<T>(asset_ptr);
    }

    std::shared_ptr<T> asset_value = std::dynamic_pointer_cast<T>(asset_pair.second->construct());

    assert(asset_value != nullptr && "Asset construction failed");

    asset_ptr = asset_value;

    return asset_value;
  }

 private:
  const std::filesystem::path m_filepath;
  std::filesystem::path m_base_dir;
  JSON m_json;
  static AssetMap m_assets;

  const std::unordered_map<std::string, AssetType> m_asset_types = {
      {ASSET_TYPE_TEXTURE, AssetType::TEXTURE},
      {ASSET_TYPE_TEXTURE_ATLAS, AssetType::TEXTURE_ATLAS},
      {ASSET_TYPE_SHADER, AssetType::SHADER},
      {ASSET_TYPE_FONT, AssetType::FONT},
  };

 private:
  void m_init_assets();
};
}  // namespace dl
