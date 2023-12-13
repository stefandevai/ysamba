#include "./animation_manager.hpp"

#include "./animation.hpp"
#include "ui/components/component.hpp"

namespace dl::ui
{
void AnimationManager::update(const double delta)
{
  const auto& fade_in_view = registry.view<AnimationFadeIn>();

  for (const auto entity : fade_in_view)
  {
    auto& animation = registry.get<AnimationFadeIn>(entity);
    assert(animation.component != nullptr);

    if (animation.time_left > 0.0)
    {
      animation.time_left -= delta;
      animation.component->opacity = std::min(1.0 - animation.time_left, 1.0);
      continue;
    }

    animation.component->opacity = 1.0;
    animation.component->state = UIComponent::State::Visible;
    registry.remove<AnimationFadeIn>(entity);
  }

  const auto& fade_out_view = registry.view<AnimationFadeOut>();

  for (const auto entity : fade_out_view)
  {
    auto& animation = registry.get<AnimationFadeOut>(entity);
    assert(animation.component != nullptr);

    if (animation.time_left > 0.0)
    {
      animation.time_left -= delta;
      animation.component->opacity = std::max(animation.time_left, 0.0);
      continue;
    }

    animation.component->opacity = 0.0;
    animation.component->state = UIComponent::State::Hidden;
    registry.remove<AnimationFadeOut>(entity);
  }
}
}  // namespace dl::ui
