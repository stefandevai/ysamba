#pragma once

#include "../core/asset.hpp"

namespace dl
{
class ShaderLoader : public AssetLoader
{
 public:
  ShaderLoader(const std::string& generic_filepath);
  virtual std::shared_ptr<Asset> construct();

 private:
  const std::string m_generic_filepath;
};
}  // namespace dl
