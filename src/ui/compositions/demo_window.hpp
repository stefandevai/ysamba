#pragma once

#include "ui/components/component.hpp"

namespace dl::ui
{
class WindowFrame;

class DemoWindow : public UIComponent
{
 public:
  DemoWindow(UIContext& context);
  void process_input() override;
  void show() override;
  void hide() override;

 private:
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
