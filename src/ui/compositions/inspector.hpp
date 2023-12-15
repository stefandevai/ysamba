#pragma once

#include "ui/components/component.hpp"

namespace dl::ui
{
class Label;
class WindowFrame;

class Inspector : public UIComponent
{
 public:
  Inspector(UIContext& context);

  void set_content(const std::string& text);
  void show();
  void hide();

 private:
  WindowFrame* m_window_frame = nullptr;
  Label* m_label = nullptr;
};

}  // namespace dl::ui
