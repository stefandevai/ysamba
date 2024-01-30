#include "./asset_manager2.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

namespace dl
{
std::unordered_map<uint32_t, Asset2> AssetManager2::m_assets{};

const std::unordered_map<std::string, AssetType2> AssetManager2::m_asset_types = {
    {"texture", AssetType2::Texture},
    {"texture_atlas", AssetType2::TextureAtlas},
    {"shader", AssetType2::Shader},
    {"font", AssetType2::Font},
};

void AssetManager2::load_assets(const std::filesystem::path& filepath)
{
  m_json.load(filepath.string());

  for (const auto& asset_info : m_json.object["assets"])
  {
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
    case AssetType2::Shader:
    {
      const auto& vertex_path = asset_info["vertex_path"].get<std::string>();
      const auto& fragment_path = asset_info["fragment_path"].get<std::string>();
      add<ShaderProgram>(hashed_id, vertex_path, fragment_path);
    }
    break;

    case AssetType2::Texture:
    {
      const auto& filepath = asset_info["path"].get<std::string>();
      add<Texture>(hashed_id, filepath);
    }
    break;

      // case AssetType2::TextureAtlas:
      // {
      //   const auto& filepath = asset_info["path"].get<std::string>();
      //   add<Texture>(hashed_id, filepath);
      // }
      // break;

    case AssetType2::Font:
    {
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
