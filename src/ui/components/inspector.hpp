#pragma once

#include "./component.hpp"

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
  std::shared_ptr<Container> m_container;
  std::shared_ptr<Label> m_label;
};

}  // namespace dl::ui
