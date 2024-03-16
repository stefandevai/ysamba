#pragma once

#include "ui/components/component.hpp"

namespace dl::ui
{
class Label;

class Notification : public UIComponent
{
 public:
  Notification(UIContext& context, const std::string& text);
  void show();
  void hide();

 private:
  Label* m_label = nullptr;
};

}  // namespace dl::ui
