#include "./item_details.hpp"

#include <spdlog/spdlog.h>

#include "core/maths/vector.hpp"
#include "ui/animation.hpp"
#include "ui/components/window_frame.hpp"

namespace dl::ui
{
ItemDetails::ItemDetails(UIContext& context) : UIComponent(context)
{
  state = UIComponent::State::Hidden;

  m_window_frame = emplace<WindowFrame>();
  m_window_frame->size = Vector2i{500, 400};
  m_window_frame->x_alignment = XAlignement::Center;
  m_window_frame->y_alignment = YAlignement::Center;
}

void ItemDetails::show()
{
  animate<AnimationFadeIn>(0.3, Easing::OutQuart);
}

void ItemDetails::hide()
{
  animate<AnimationFadeOut>(0.3, Easing::OutQuart);
}

}  // namespace dl::ui
