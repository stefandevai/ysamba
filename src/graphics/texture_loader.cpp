#include "./texture_loader.hpp"

namespace dl
{
// Load single texture
TextureLoader::TextureLoader(const std::string& filepath, const TextureType type)
    : AssetLoader(AssetType::TEXTURE),
      m_filepath(filepath),
      m_type(type),
      m_mode(TextureMode::SINGLE),
      m_horizontal_frames(1),
      m_vertical_frames(1)
{
}

// Load uniform texture atlas
TextureLoader::TextureLoader(const std::string& filepath,
                             const TextureType type,
                             const int horizontal_frames,
                             const int vertical_frames)
    : AssetLoader(AssetType::TEXTURE),
      m_filepath(filepath),
      m_type(type),
      m_mode(TextureMode::UNIFORM_ATLAS),
      m_horizontal_frames(horizontal_frames),
      m_vertical_frames(vertical_frames)
{
}

// Load uniform texture atlas with associated data
TextureLoader::TextureLoader(const std::string& filepath,
                             const TextureType type,
                             const int horizontal_frames,
                             const int vertical_frames,
                             const std::string& data_filepath)
    : AssetLoader(AssetType::TEXTURE),
      m_filepath(filepath),
      m_type(type),
      m_mode(TextureMode::UNIFORM_ATLAS),
      m_horizontal_frames(horizontal_frames),
      m_vertical_frames(vertical_frames),
      m_data_filepath(data_filepath)
{
}

std::shared_ptr<Asset> TextureLoader::construct()
{
  switch (m_mode)
  {
  case TextureMode::SINGLE:
  {
    return std::make_shared<Texture>(m_filepath, m_type);
  }
  break;

  case TextureMode::UNIFORM_ATLAS:
  {
    const auto texture = std::make_shared<Texture>(m_filepath, m_type, m_horizontal_frames, m_vertical_frames);

    if (!m_data_filepath.empty())
    {
      texture->load_data(m_data_filepath);
    }

    return texture;
  }
  break;

  default:
    break;
  }

  // Remove warning about non void method without return
  return std::make_shared<Texture>(m_filepath, m_type);
}
}  // namespace dl
