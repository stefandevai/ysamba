#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
Inspector::Inspector(UIContext& context) : UIComponent(context)
{
  state = UIComponent::State::Hidden;
  size = Vector2i{250, 100};
  position = Vector3i{30, 30, 0};

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = size;

  m_label = m_window_frame->emplace<Label>("");
  m_label->position = Vector3i{20, 20, 0};

  placement = Placement::Absolute;
  x_alignment = XAlignement::Right;
}

void Inspector::set_content(const std::string& text)
{
  dirty = true;
  m_label->set_text(text);
}

void Inspector::show()
{
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void Inspector::hide()
{
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

}  // namespace dl::ui
