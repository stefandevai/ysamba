#pragma once

#include <webgpu/webgpu.h>

#include <filesystem>

namespace dl
{
class Shader
{
 public:
  WGPUShaderModule module;

  Shader() = default;
  ~Shader();

  void load(WGPUDevice device, const std::filesystem::path& filepath);

 private:
  bool m_has_loaded = false;
};
};  // namespace dl
