#pragma once

#include "./component.hpp"
#include "core/maths/vector.hpp"
#include "core/timer.hpp"
#include "graphics/nine_patch.hpp"

namespace dl::ui
{
class Label;
class Container;
class NinePatchContainer;

class TextInput : public UIComponent
{
 public:
  struct Params
  {
    std::string text{};
    std::string placeholder{};
    Vector2i size{250, 36};
  };

  std::string text{};
  std::string placeholder{};
  int cursor_index{};

  NinePatchContainer* container = nullptr;
  Label* label = nullptr;
  Container* cursor = nullptr;

  TextInput(UIContext& context, Params params);

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

  InputState m_state = InputState::Display;
  CursorState m_cursor_state = CursorState::Display;
  Timer m_cursor_timer{};
};

}  // namespace dl::ui
