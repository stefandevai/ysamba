#pragma once

#include "./texture.hpp"
#include "core/asset.hpp"

namespace dl
{
enum class TextureMode
{
  NONE,
  SINGLE,
  UNIFORM_ATLAS,
};

class TextureLoader : public AssetLoader
{
 public:
  // Load single texture
  TextureLoader(const std::string& filepath);

  // Load uniform texture atlas
  TextureLoader(const std::string& filepath, const int horizontal_frames, const int vertical_frames);

  // Load uniform texture atlas with associated data
  TextureLoader(const std::string& filepath,
                const int horizontal_frames,
                const int vertical_frames,
                const std::string& data_filepath);

  virtual std::shared_ptr<Asset> construct();

 private:
  const std::string m_filepath;
  const TextureMode m_mode;
  const int m_horizontal_frames;
  const int m_vertical_frames;
  const std::string m_data_filepath;
};
}  // namespace dl
