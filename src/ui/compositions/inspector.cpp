#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/label.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
Inspector::Inspector(UIContext& context) : UIComponent(context)
{
  state = UIComponent::State::Hidden;
  size = {250, 100};
  position = {30, 30, 0};

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = size;

  m_label = m_window_frame->emplace<Label>("");
  m_label->position = {20, 20, 0};

  placement = Placement::Absolute;
  x_alignment = XAlignement::Right;
}

void Inspector::set_content(const std::string& text)
{
  dirty = true;
  m_label->text.set_text(text);
}

}  // namespace dl::ui
