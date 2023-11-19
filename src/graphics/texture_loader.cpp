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
    return std::make_shared<Texture>(m_filepath, m_type, m_horizontal_frames, m_vertical_frames);
  }
  break;

  default:
    break;
  }

  // Remove warning about non void method without return
  return std::make_shared<Texture>(m_filepath, m_type);
}
}  // namespace dl
