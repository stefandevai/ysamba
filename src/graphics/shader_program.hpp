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
  bool has_loaded = false;

  ShaderProgram(const char* vertex_source, const char* fragment_source);
  ShaderProgram(const std::string& vertex_path, const std::string& fragment_path);
  ~ShaderProgram();

  void load();
  void use();
  void set_float(const std::string& uniform_name, const float value);
  void set_int(const std::string& uniform_name, const int value);
  void set_mat4(const std::string& uniform_name, const glm::mat4& mat);
  GLint get_uniform_location(const std::string& uniform_name);

 private:
  int m_program;
  std::string m_vertex_path{};
  std::string m_fragment_path{};
  std::unordered_map<std::string, GLint> m_cached_uniform_locations{};
};
}  // namespace dl
