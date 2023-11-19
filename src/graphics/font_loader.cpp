#include "./font_loader.hpp"

#include "./font.hpp"

namespace dl
{
FontLoader::FontLoader(const std::string& filepath, const std::size_t size)
    : AssetLoader(AssetType::FONT), m_filepath(filepath), m_size(size)
{
}

std::shared_ptr<Asset> FontLoader::construct() { return std::make_shared<Font>(m_filepath, m_size); }

}  // namespace dl
