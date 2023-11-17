#pragma once

namespace dl
{
class ShaderProgram;

class Renderable
{
public:
  virtual ~Renderable() {}

  virtual void render (ShaderProgram& shader) = 0;
};
}
