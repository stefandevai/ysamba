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

  Texture(WGPUDevice device, const unsigned char* data, const Vector2i& size, int channels = 4);
  Texture(const std::string& filepath);
  ~Texture();

  static Texture dummy(WGPUDevice device);

  // Loads after setting filepath
  void load(WGPUDevice device);

  // Loads texture from data
  void load(WGPUDevice device, const unsigned char* data, const Vector2i& size, int channels);
};
}  // namespace dl
