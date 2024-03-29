#pragma once

#include "./component.hpp"
#include "core/timer.hpp"
#include "graphics/quad.hpp"

namespace dl::ui
{
class Label;
class Container;

class TextInput : public UIComponent
{
 public:
  std::string text = "";
  std::string placeholder = "";
  int cursor_index{};

  TextInput(UIContext& context);

  void init();
  void update();

 private:
  enum class InputState
  {
    Display,
    Focus,
  };

  enum class CursorState
  {
    Display,
    Hide,
  };

  Container* m_container = nullptr;
  Label* m_label = nullptr;
  InputState m_state = InputState::Display;
  CursorState m_cursor_state = CursorState::Display;
  Container* m_cursor = nullptr;
  Timer m_cursor_timer{};
};

}  // namespace dl::ui
