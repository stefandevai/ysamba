#include "./texture.hpp"

#include <spdlog/spdlog.h>

#include "core/json.hpp"

extern "C"
{
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

namespace dl
{
// Create full sized texture
Texture::Texture(const std::string& filepath) : filepath(filepath) {}

// Load texture from data on constructor
Texture::Texture(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels)
{
  load(device, data, size, channels);
}

Texture::~Texture()
{
  if (has_loaded)
  {
    wgpuTextureViewRelease(view);
    wgpuTextureDestroy(texture);
    wgpuTextureRelease(texture);
  }
}

Texture Texture::dummy(const WGPUDevice device)
{
  static constexpr unsigned char dummy_data = 0;
  Texture dummy_texture{device, &dummy_data, Vector2i{1, 1}, 1};
  return dummy_texture;
}

void Texture::load(const WGPUDevice device)
{
  assert(!filepath.empty() && "Texture filepath is empty");

  int width = 0;
  int height = 0;
  int channels = 0;
  unsigned char* image_data;

  // image_data = stbi_load (filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  image_data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

  if (image_data == nullptr)
  {
    spdlog::critical("Failed to load texture: {}", filepath);
    return;
  }

  load(device, image_data, Vector2i{width, height}, channels);
  stbi_image_free(image_data);
}

void Texture::load(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels)
{
  this->size = size;

  const auto queue = wgpuDeviceGetQueue(device);

  WGPUTextureFormat format;
  switch (channels)
  {
  case 1:
    format = WGPUTextureFormat_R8Unorm;
    break;

  case 4:
    format = WGPUTextureFormat_RGBA8Unorm;
    break;

  default:
    format = WGPUTextureFormat_RGBA8Unorm;
    break;
  }

  WGPUTextureDescriptor textureDesc{
      .label = "WorldPipeline Texture",
      .dimension = WGPUTextureDimension_2D,
      .size = {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), 1},
      .mipLevelCount = 1,
      .sampleCount = 1,
      .format = format,
      .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
      .viewFormatCount = 0,
      .viewFormats = nullptr,
  };

  texture = wgpuDeviceCreateTexture(device, &textureDesc);
  assert(texture != nullptr);

  WGPUImageCopyTexture destination = {
      .texture = texture,
      .mipLevel = 0,
      .origin = {0, 0, 0},
      .aspect = WGPUTextureAspect_All,
  };

  WGPUTextureDataLayout source = {
      .offset = 0,
      .bytesPerRow = channels * textureDesc.size.width,
      .rowsPerImage = textureDesc.size.height,
  };

  wgpuQueueWriteTexture(
      queue, &destination, data, size.x * size.y * channels * sizeof(unsigned char), &source, &textureDesc.size);

  WGPUTextureViewDescriptor textureViewDesc{
      .label = "WorldPipeline Texture View",
      .aspect = WGPUTextureAspect_All,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .dimension = WGPUTextureViewDimension_2D,
      .format = textureDesc.format,
  };
  view = wgpuTextureCreateView(texture, &textureViewDesc);
  assert(view != nullptr);

  has_loaded = true;
}

}  // namespace dl
