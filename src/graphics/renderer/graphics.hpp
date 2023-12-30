#pragma once

#include <cstdint>

namespace dl::v2
{
class Shader
{
 public:
  virtual ~Shader() = default;
  static Shader& create(const ShaderData& data) = 0;
};

class Texture
{
 public:
  virtual ~Texture() = default;
  static Texture& create(const int width, const int height, uint8_t* data = nullptr) = 0;
  static Texture& create(const TextureData& data) = 0;
};

class Target
{
};

class Material
{
};
}  // namespace dl::v2
