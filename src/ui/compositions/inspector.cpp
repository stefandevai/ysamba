#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
Inspector::Inspector(UIContext& context, Params params) : UIComponent(context, "Inspector")
{
  state = UIComponent::State::Hidden;
  size = Vector2i{250, 100};
  position = Vector3i{30, 30, 0};
  placement = Placement::Absolute;
  x_alignment = XAlignement::Right;

  m_window_frame = emplace<WindowFrame>(WindowFrame::Params{
      .size = size,
      .has_close_button = false,
  });

  m_label = m_window_frame->emplace<Label>(std::move(params.content));
  m_label->position = Vector3i{20, 20, 0};
}

void Inspector::set_content(const std::string text)
{
  m_label->set_text(std::move(text));
  dirty = true;
}

void Inspector::show()
{
  animate<AnimationFadeIn>(0.2, Easing::OutCirc);
  animate<AnimationMoveFrom>(0.2, Easing::OutQuart, AnimationMovePosition::Right);
}

void Inspector::hide()
{
  animate<AnimationFadeOut>(0.1, Easing::InCirc);
  animate<AnimationMoveTo>(0.1, Easing::InQuart, AnimationMovePosition::Right);
}

}  // namespace dl::ui
