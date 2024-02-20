#include "./shader.hpp"

#include "core/utils.hpp"

namespace dl
{
Shader::~Shader()
{
  if (m_has_loaded)
  {
    wgpuShaderModuleRelease(module);
  }
}

void Shader::load(WGPUDevice device, const std::filesystem::path& filepath)
{
  const auto shader_source = utils::read_file(filepath.string());

  WGPUShaderModuleDescriptor shader_descriptor = {};
  shader_descriptor.nextInChain = nullptr;
  shader_descriptor.hintCount = 0;
  shader_descriptor.hints = nullptr;

  // Use the extension mechanism to load a WGSL shader source code
  WGPUShaderModuleWGSLDescriptor shaderCodeDesc = {};
  // Set the chained struct's header
  shaderCodeDesc.chain.next = nullptr;
  shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
  // Connect the chain
  shader_descriptor.nextInChain = &shaderCodeDesc.chain;

  // Setup the actual payload of the shader code descriptor
  shaderCodeDesc.code = shader_source.c_str();

  module = wgpuDeviceCreateShaderModule(device, &shader_descriptor);

  m_has_loaded = true;
}
}  // namespace dl
