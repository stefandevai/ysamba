#include "./animation_manager.hpp"

#include "./animation.hpp"
#include "ui/components/component.hpp"

namespace dl::ui
{
void AnimationManager::update(const double delta)
{
  const auto& fade_in_view = registry.view<AnimationFadeIn, AnimationTarget>();

  for (const auto entity : fade_in_view)
  {
    auto& animation = registry.get<AnimationFadeIn>(entity);
    auto& target = registry.get<AnimationTarget>(entity);

    assert(target.value != nullptr);

    if (animation.time_spent < animation.duration)
    {
      animation.time_spent += delta;
      const auto t = std::min(animation.time_spent / animation.duration, 1.0);
      target.value->opacity = animation::get_ease_value(t, animation.easing);
      continue;
    }

    target.value->opacity = 1.0;
    target.value->state = UIComponent::State::Visible;
    registry.remove<AnimationFadeIn>(entity);
  }

  const auto& fade_out_view = registry.view<AnimationFadeOut, AnimationTarget>();

  for (const auto entity : fade_out_view)
  {
    auto& animation = registry.get<AnimationFadeOut>(entity);
    auto& target = registry.get<AnimationTarget>(entity);

    assert(target.value != nullptr);

    if (animation.time_spent < animation.duration)
    {
      animation.time_spent += delta;
      const auto t = std::min(animation.time_spent / animation.duration, 1.0);
      target.value->opacity = 1.0 - animation::get_ease_value(t, animation.easing);
      continue;
    }

    target.value->opacity = 0.0;
    target.value->state = UIComponent::State::Hidden;
    registry.remove<AnimationFadeOut>(entity);
  }
}
}  // namespace dl::ui
