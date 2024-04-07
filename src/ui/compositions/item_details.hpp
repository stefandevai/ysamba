#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
class WindowFrame;

class ItemDetails : public UIComponent
{
 public:
  ItemDetails(UIContext& context);

  void show();
  void hide();

 private:
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
