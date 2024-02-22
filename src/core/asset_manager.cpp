#include "./asset_manager.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/json.hpp"

namespace dl
{
std::unordered_map<uint32_t, Asset> AssetManager::m_assets{};

const std::unordered_map<std::string, AssetType> AssetManager::m_asset_types = {
    {"texture", AssetType::Texture},
    {"texture_atlas", AssetType::TextureAtlas},
    {"shader", AssetType::Shader},
    {"font", AssetType::Font},
};

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
    case AssetType::Shader:
    {
      assert(asset_info.contains("vertex_path") && "Vertex path not specified");
      assert(asset_info.contains("fragment_path") && "Fragment path not specified");
      const auto& vertex_path = asset_info["vertex_path"].get<std::string>();
      const auto& fragment_path = asset_info["fragment_path"].get<std::string>();
      add<ShaderProgram>(hashed_id, vertex_path, fragment_path);
    }
    break;

    case AssetType::Texture:
    {
      assert(asset_info.contains("path") && "Texture path not specified");
      const auto& filepath = asset_info["path"].get<std::string>();
      add<v2::Texture>(hashed_id, filepath);
    }
    break;

    case AssetType::TextureAtlas:
    {
      assert(asset_info.contains("path") && "Texture atlas path not specified");
      assert(asset_info.contains("horizontal_frames") && "Texture atlas horizontal_frames not specified");
      assert(asset_info.contains("vertical_frames") && "Texture atlas vertical_frames not specified");

      const auto& filepath = asset_info["path"].get<std::string>();
      const auto horizontal_frames = asset_info["horizontal_frames"].get<int>();
      const auto vertical_frames = asset_info["vertical_frames"].get<int>();

      if (asset_info.contains("data_path"))
      {
        const auto data_filepath = asset_info["data_path"].get<std::string>();
        add<v2::Texture>(hashed_id, filepath, horizontal_frames, vertical_frames, data_filepath);
        break;
      }

      add<v2::Texture>(hashed_id, filepath, horizontal_frames, vertical_frames);
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
