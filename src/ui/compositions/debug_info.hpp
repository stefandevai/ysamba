#pragma once

#include "ui/components/component.hpp"

namespace dl
{
class Camera;
}

namespace dl::ui
{
class Label;
class Container;

class DebugInfo : public UIComponent
{
 public:
  DebugInfo(UIContext& context, const Camera& camera);

  void update();
  /* void set_content(const std::string_view text); */

 private:
  const Camera& m_camera;
  Container* m_container;
  Label* m_label;
  std::string m_fps = "FPS: ";
  double m_delay = 0.0;
};

}  // namespace dl::ui
