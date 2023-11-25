#pragma once

#include "core/asset.hpp"

namespace dl
{
class FontLoader : public AssetLoader
{
 public:
  FontLoader(const std::string& filepath, const std::size_t size);
  virtual std::shared_ptr<Asset> construct();

 private:
  const std::string m_filepath;
  std::size_t m_size = 48;
};
}  // namespace dl
