#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "core/asset.hpp"

namespace dl
{
class ShaderProgram : public Asset
{
 public:
  ShaderProgram(const char* vertex_source, const char* fragment_source);
  ~ShaderProgram();

  void use();
  void set_float(const std::string& uniform_name, const float value) const;
  void set_int(const std::string& uniform_name, const int value) const;
  void set_mat_4(const std::string& uniform_name, const glm::mat4& mat) const;

 private:
  int m_program;
};
}  // namespace dl
