#pragma once

#include "ui/components/component.hpp"

namespace dl::ui
{
class Label;

class Notification : public UIComponent
{
 public:
  struct Params
  {
    const std::string text{};
  };

  Notification(UIContext& context, const std::string& text);
  void show() override;
  void hide() override;

 private:
  Label* m_label = nullptr;
};

}  // namespace dl::ui
