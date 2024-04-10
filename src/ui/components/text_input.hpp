#pragma once

#include "./component.hpp"
#include "core/maths/vector.hpp"
#include "core/timer.hpp"
#include "graphics/nine_patch.hpp"
#include "graphics/quad.hpp"

namespace dl::ui
{
class Label;
class Container;

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

  TextInput(UIContext& context, Params params);

  void update();
  void render();

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

  NinePatch nine_patch{};
  Label* m_label = nullptr;
  InputState m_state = InputState::Display;
  CursorState m_cursor_state = CursorState::Display;
  Container* m_cursor = nullptr;
  Timer m_cursor_timer{};
};

}  // namespace dl::ui
