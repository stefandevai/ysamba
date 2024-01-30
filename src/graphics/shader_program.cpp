#include "./shader_program.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "core/fileutils.hpp"

namespace dl
{
ShaderProgram::ShaderProgram(const char* vertex_source, const char* fragment_source)
{
  int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source, NULL);
  glCompileShader(vertex_shader);

  int success;
  char info_log[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

  if (!success)
  {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    spdlog::critical("Vertex Shader compilation failed: {}\n{}", vertex_source, info_log);
  }

  int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    spdlog::critical("Fragment Shader compilation failed: {}\n{}", fragment_source, info_log);
  }

  m_program = glCreateProgram();
  glAttachShader(m_program, vertex_shader);
  glAttachShader(m_program, fragment_shader);
  glLinkProgram(m_program);

  glGetProgramiv(m_program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(m_program, 512, NULL, info_log);
    spdlog::critical("Program linking failed: {} {}\n{}", vertex_source, fragment_source, info_log);
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

ShaderProgram::ShaderProgram(const std::string& vertex_path, const std::string& fragment_path)
    : m_vertex_path(vertex_path), m_fragment_path(fragment_path)
{
}

void ShaderProgram::load()
{
  if (m_vertex_path.empty() || m_fragment_path.empty())
  {
    spdlog::critical("Vertex or fragment path is empty");
    return;
  }

  const auto vertex_source = FileUtils::read_file(m_vertex_path).c_str();
  const auto fragment_source = FileUtils::read_file(m_fragment_path).c_str();

  int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source, NULL);
  glCompileShader(vertex_shader);

  int success;
  char info_log[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

  if (!success)
  {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    spdlog::critical("Vertex Shader compilation failed: {}\n{}", vertex_source, info_log);
  }

  int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    spdlog::critical("Fragment Shader compilation failed: {}\n{}", fragment_source, info_log);
  }

  m_program = glCreateProgram();
  glAttachShader(m_program, vertex_shader);
  glAttachShader(m_program, fragment_shader);
  glLinkProgram(m_program);

  glGetProgramiv(m_program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(m_program, 512, NULL, info_log);
    spdlog::critical("Program linking failed: {} {}\n{}", vertex_source, fragment_source, info_log);
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  has_loaded = true;
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(m_program); }

void ShaderProgram::use() { glUseProgram(m_program); }

void ShaderProgram::set_mat4(const std::string& uniform_name, const glm::mat4& mat)
{
  glUniformMatrix4fv(get_uniform_location(uniform_name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::set_float(const std::string& uniform_name, const float value)
{
  glUniform1f(get_uniform_location(uniform_name), value);
}

void ShaderProgram::set_int(const std::string& uniform_name, const int value)
{
  glUniform1i(get_uniform_location(uniform_name), value);
}

GLint ShaderProgram::get_uniform_location(const std::string& uniform_name)
{
  if (m_cached_uniform_locations.contains(uniform_name))
  {
    return m_cached_uniform_locations.at(uniform_name);
  }

  GLint location = glGetUniformLocation(m_program, uniform_name.c_str());
  m_cached_uniform_locations.insert({uniform_name, location});

  return location;
}
}  // namespace dl
