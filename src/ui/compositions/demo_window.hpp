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
  void process_input() override;
  void show() override;
  void hide() override;

 private:
  ScrollableTextButtonList<uint32_t>* m_list = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
