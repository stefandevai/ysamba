#pragma once

namespace dl
{
class RenderSystem;

class RenderEditor
{
 public:
  RenderEditor(RenderSystem& render);
  void update();

 private:
  RenderSystem& m_render;
  bool m_open = true;
};
}  // namespace dl
