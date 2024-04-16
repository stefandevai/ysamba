#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class ScrollableTextButtonList;
class WindowFrame;

class DemoWindow : public UIComponent
{
 public:
  DemoWindow(UIContext& context);
  void show();
  void hide();

 private:
  ScrollableTextButtonList<uint32_t>* m_list = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
