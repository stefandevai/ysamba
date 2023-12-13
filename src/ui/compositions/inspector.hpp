#pragma once

#include "ui/components/component.hpp"

namespace dl::ui
{
class Label;
class Container;

class Inspector : public UIComponent
{
 public:
  Inspector();

  void set_content(const std::string& text);

 private:
  Container* m_container;
  Label* m_label;
};

}  // namespace dl::ui
