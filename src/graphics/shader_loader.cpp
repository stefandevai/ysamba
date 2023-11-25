#include "./shader_loader.hpp"

#include "./shader_program.hpp"
#include "core/fileutils.hpp"

namespace dl
{
ShaderLoader::ShaderLoader(const std::string& generic_filepath)
    : AssetLoader(AssetType::SHADER), m_generic_filepath(generic_filepath)
{
}

std::shared_ptr<Asset> ShaderLoader::construct()
{
  const auto vertex_source = FileUtils::read_file(m_generic_filepath + ".vs");
  const auto fragment_source = FileUtils::read_file(m_generic_filepath + ".fs");
  return std::make_shared<ShaderProgram>(vertex_source.c_str(), fragment_source.c_str());
}
}  // namespace dl
