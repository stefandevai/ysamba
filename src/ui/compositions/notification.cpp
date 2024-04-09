#include "./notification.hpp"

#include <spdlog/spdlog.h>

#include "ui/animation.hpp"
#include "ui/components/label.hpp"

namespace dl::ui
{
Notification::Notification(UIContext& context, const std::string& text) : UIComponent(context, "Notification")
{
  state = UIComponent::State::Hidden;

  m_label = emplace<Label>(text);

  size = m_label->size;
  x_alignment = ui::XAlignement::Center;
  position.y = 30;
}

void Notification::show()
{
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void Notification::hide()
{
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

}  // namespace dl::ui
