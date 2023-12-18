#pragma once

#include "./component.hpp"

namespace dl::ui
{
class Label;
class Container;

class TextInput : public UIComponent
{
 public:
  std::string text = "Initial state";
  TextInput(UIContext& context);

  void init();
  void update();

 private:
  enum class State
  {
    Display,
    Focus,
  };

  Container* m_container = nullptr;
  Label* m_label = nullptr;
  State m_state = State::Display;
};

}  // namespace dl::ui
