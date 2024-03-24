#include "./texture_atlas.hpp"

#include <spdlog/spdlog.h>

#include "core/json.hpp"

namespace dl
{
TextureAtlas::TextureAtlas(const std::string& filepath, const std::string& data_filepath)
    : texture(std::make_unique<Texture>(filepath)), m_data_filepath(data_filepath)
{
}

void TextureAtlas::load(const WGPUDevice device)
{
  texture->load(device);
  m_load_metadata(m_data_filepath);
  has_loaded = true;
}

void TextureAtlas::m_load_metadata(const std::string& filepath)
{
  JSON json{filepath};

  if (!json.object.contains("frames"))
  {
    spdlog::warn("Trying to load tileset data without frames: {}", filepath);
    return;
  }

  const auto items = json.object["frames"].get<std::vector<nlohmann::json>>();

  for (const auto& item : items)
  {
    const auto game_id = item["game_id"].get<uint32_t>();
    const auto type = item["type"].get<std::string>();

    if (type == "nine_patch")
    {
      const auto top = item["top"].get<uint32_t>();
      const auto left = item["left"].get<uint32_t>();
      const auto bottom = item["bottom"].get<uint32_t>();
      const auto right = item["right"].get<uint32_t>();
      const auto border = item["border"].get<uint32_t>();

      m_frame_data.emplace(game_id, NinePatchData{top, left, bottom, right, border});
    }
  }

  has_metadata = true;
}

}  // namespace dl
