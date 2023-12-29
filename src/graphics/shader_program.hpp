#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <unordered_map>

#include "core/asset.hpp"

namespace dl
{
class ShaderProgram : public Asset
{
 public:
  ShaderProgram(const char* vertex_source, const char* fragment_source);
  ~ShaderProgram();

  void use();
  void set_float(const std::string& uniform_name, const float value);
  void set_int(const std::string& uniform_name, const int value);
  void set_mat4(const std::string& uniform_name, const glm::mat4& mat);
  GLint get_uniform_location(const std::string& uniform_name);

 private:
  int m_program;
  std::unordered_map<std::string, GLint> m_cached_uniform_locations{};
};
}  // namespace dl
