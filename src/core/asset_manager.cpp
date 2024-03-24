#include "./asset_manager.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/json.hpp"

namespace dl
{
void AssetManager::load_assets(const std::filesystem::path& filepath)
{
  JSON json{filepath.string()};

  for (const auto& asset_info : json.object["assets"])
  {
    assert(asset_info.contains("id") && "Asset id not specified");
    assert(asset_info.contains("type") && "Asset type not specified");

    const auto& id = asset_info["id"].get<std::string>();
    const uint32_t hashed_id = entt::hashed_string::value(id.c_str());
    const auto& type_tag = asset_info["type"].get<std::string>();

    if (!m_asset_types.contains(type_tag))
    {
      spdlog::warn("Asset type {} not supported", type_tag);
      continue;
    }

    const auto type = m_asset_types.at(type_tag);

    switch (type)
    {
    case AssetType::Texture:
    {
      assert(asset_info.contains("path") && "Texture path not specified");
      const auto& filepath = asset_info["path"].get<std::string>();
      add<Spritesheet>(hashed_id, filepath);
    }
    break;

    case AssetType::TextureAtlas:
    {
      assert(asset_info.contains("path") && "Texture atlas path not specified");
      assert(asset_info.contains("data_path") && "Texture atlas data path not specified");

      const auto& filepath = asset_info["path"].get<std::string>();
      const auto data_filepath = asset_info["data_path"].get<std::string>();

      add<Spritesheet>(hashed_id, filepath, data_filepath);
    }
    break;

    case AssetType::Font:
    {
      assert(asset_info.contains("path") && "Font path not specified");
      assert(asset_info.contains("size") && "Font size not specified");

      const auto filepath = asset_info["path"].get<std::string>();
      const auto size = asset_info["size"].get<std::size_t>();
      add<Font>(hashed_id, filepath, size);
    }
    break;

    default:
    {
      spdlog::warn("Asset type {} not supported", static_cast<int>(type));
    }
    break;
    }
  }
}

}  // namespace dl
