#include "./animation_manager.hpp"

#include <spdlog/spdlog.h>

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

    if (animation.time_spent == 0.0)
    {
      target.value->propagate_state();
    }

    if (animation.time_spent < animation.duration)
    {
      animation.time_spent += delta;
      const auto t = std::min(animation.time_spent / animation.duration, 1.0);
      target.value->opacity = animation::get_ease_value(t, animation.easing);
      continue;
    }

    target.value->opacity = 1.0;
    target.value->state = UIComponent::State::Visible;
    target.value->propagate_state();
    registry.remove<AnimationFadeIn>(entity);
  }

  const auto& fade_out_view = registry.view<AnimationFadeOut, AnimationTarget>();

  for (const auto entity : fade_out_view)
  {
    auto& animation = registry.get<AnimationFadeOut>(entity);
    auto& target = registry.get<AnimationTarget>(entity);

    assert(target.value != nullptr);

    if (animation.time_spent == 0.0)
    {
      target.value->propagate_state();
    }

    if (animation.time_spent < animation.duration)
    {
      animation.time_spent += delta;
      const auto t = std::min(animation.time_spent / animation.duration, 1.0);
      target.value->opacity = 1.0 - animation::get_ease_value(t, animation.easing);
      continue;
    }

    target.value->opacity = 1.0;
    target.value->state = UIComponent::State::Hidden;
    target.value->propagate_state();
    registry.remove<AnimationFadeOut>(entity);
  }

  const auto& appear_view = registry.view<AnimationAppear, AnimationTarget>();

  for (const auto entity : appear_view)
  {
    auto& animation = registry.get<AnimationAppear>(entity);
    auto& target = registry.get<AnimationTarget>(entity);

    assert(target.value != nullptr);

    // Set up initial state
    if (animation.time_spent == 0.0)
    {
      target.value->propagate_state();

      animation.original_position = target.value->position;

      if (animation.start_position == AnimationNullPosition)
      {
        switch (animation.from)
        {
        case AnimationMovePosition::Top:
        {
          animation.position_variation.y = target.value->size.y / 2;
          break;
        }
        case AnimationMovePosition::Bottom:
        {
          animation.position_variation.y = -target.value->size.y / 2;
          break;
        }
        case AnimationMovePosition::Left:
        {
          animation.position_variation.x = -target.value->size.x / 2;
          break;
        }
        case AnimationMovePosition::Right:
        {
          animation.position_variation.x = target.value->size.x / 2;
          break;
        }
        default:
          break;
        }

        animation.start_position = target.value->position - animation.position_variation;
      }
      else
      {
        animation.position_variation = target.value->position - animation.start_position;
      }
    }

    // Animate during the duration
    if (animation.time_spent < animation.duration)
    {
      animation.time_spent += delta;
      const auto t = std::min(animation.time_spent / animation.duration, 1.0);
      const auto progress = animation::get_ease_value(t, animation.easing);
      target.value->position = animation.start_position + animation.position_variation * progress;
      target.value->dirty = true;
      continue;
    }

    // Restore original state
    target.value->position = animation.original_position;
    target.value->dirty = true;

    target.value->state = UIComponent::State::Visible;
    target.value->propagate_state();

    registry.remove<AnimationAppear>(entity);
  }
}
}  // namespace dl::ui
