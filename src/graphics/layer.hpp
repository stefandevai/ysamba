#pragma once

#include <string>

#include "./renderable.hpp"

namespace dl
{
class ShaderProgram;

class Layer
{
 public:
  std::shared_ptr<ShaderProgram> shader;
  int priority = 0;

  Layer(std::shared_ptr<ShaderProgram> shader, const int priority = 0) : shader(shader), priority(priority) {}
  virtual ~Layer() {}

  virtual void render() = 0;
  virtual void emplace(const std::shared_ptr<Renderable>& renderable,
                       const double x,
                       const double y,
                       const double z) = 0;
  virtual void init_emplacing() = 0;
  virtual void finalize_emplacing() = 0;
  virtual bool get_should_render() = 0;
  bool get_ignore_camera() const { return m_ignore_camera; }
  void set_ignore_camera(const bool ignore_camera) { m_ignore_camera = ignore_camera; }

 protected:
  bool m_ignore_camera = false;
  Layer() {}
};

/* struct LayerPriorityCompare */
/* { */
/*   bool operator()(const Layer& lhs, const Layer& rhs) const */
/*   { */
/*     return lhs.priority < rhs.priority; */
/*   } */
/* }; */
}  // namespace dl
