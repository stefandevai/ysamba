#include "./shader.hpp"

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
  const char* shaderSource = R"(
@vertex
fn vs_main(@location(0) in_vertex_position: vec2f) -> @builtin(position) vec4f {
	return vec4f(in_vertex_position, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    return vec4f(0.3, 0.6, 0.9, 1.0);
}
)";

  WGPUShaderModuleDescriptor shaderDesc = {};
  shaderDesc.nextInChain = nullptr;
  shaderDesc.hintCount = 0;
  shaderDesc.hints = nullptr;

  // Use the extension mechanism to load a WGSL shader source code
  WGPUShaderModuleWGSLDescriptor shaderCodeDesc = {};
  // Set the chained struct's header
  shaderCodeDesc.chain.next = nullptr;
  shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
  // Connect the chain
  shaderDesc.nextInChain = &shaderCodeDesc.chain;

  // Setup the actual payload of the shader code descriptor
  shaderCodeDesc.code = shaderSource;

  module = wgpuDeviceCreateShaderModule(device, &shaderDesc);

  m_has_loaded = true;
}
}  // namespace dl
