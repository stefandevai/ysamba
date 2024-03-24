#pragma once

#include <webgpu/wgpu.h>

#include <string>

#include "core/maths/vector.hpp"

namespace dl
{
class Texture
{
 public:
  std::string filepath{};
  Vector2i size{};
  bool has_loaded = false;

  WGPUTexture texture;
  WGPUTextureView view;

  Texture(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels = 4);
  Texture(const std::string& filepath);
  ~Texture();

  static Texture dummy(const WGPUDevice device);

  // Loads after setting filepath
  void load(const WGPUDevice device);

  // Loads texture from data
  void load(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels);
};
}  // namespace dl
