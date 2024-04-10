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
    registry.remove<AnimationFadeIn>(entity);

    // Check if other animations are still playing
    auto& counter = registry.get<AnimationCounter>(entity);

    if (counter.count > 1)
    {
      --counter.count;
    }
    else
    {
      registry.remove<AnimationCounter>(entity);
      target.value->state = UIComponent::State::Visible;
      target.value->propagate_state();
    }
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
    registry.remove<AnimationFadeOut>(entity);

    // Check if other animations are still playing
    auto& counter = registry.get<AnimationCounter>(entity);

    if (counter.count > 1)
    {
      --counter.count;
    }
    else
    {
      registry.remove<AnimationCounter>(entity);
      target.value->state = UIComponent::State::Hidden;
      target.value->propagate_state();
    }
  }

  const auto& move_from_view = registry.view<AnimationMoveFrom, AnimationTarget>();

  for (const auto entity : move_from_view)
  {
    auto& animation = registry.get<AnimationMoveFrom>(entity);
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
    registry.remove<AnimationMoveFrom>(entity);

    // Check if other animations are still playing
    auto& counter = registry.get<AnimationCounter>(entity);

    if (counter.count > 1)
    {
      --counter.count;
    }
    else
    {
      registry.remove<AnimationCounter>(entity);
      target.value->state = UIComponent::State::Visible;
      target.value->propagate_state();
    }
  }

  const auto& move_to_view = registry.view<AnimationMoveTo, AnimationTarget>();

  for (const auto entity : move_to_view)
  {
    auto& animation = registry.get<AnimationMoveTo>(entity);
    auto& target = registry.get<AnimationTarget>(entity);

    assert(target.value != nullptr);

    // Set up initial state
    if (animation.time_spent == 0.0)
    {
      target.value->propagate_state();

      animation.original_position = target.value->position;

      if (animation.end_position == AnimationNullPosition)
      {
        switch (animation.to)
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

        animation.end_position = target.value->position - animation.position_variation;
      }
      else
      {
        animation.position_variation = target.value->position - animation.end_position;
      }
    }

    // Animate during the duration
    if (animation.time_spent < animation.duration)
    {
      animation.time_spent += delta;
      const auto t = std::min(animation.time_spent / animation.duration, 1.0);
      const auto progress = animation::get_ease_value(t, animation.easing);
      target.value->position = animation.original_position - animation.position_variation * progress;
      target.value->dirty = true;
      continue;
    }

    // Restore original state
    target.value->position = animation.original_position;
    target.value->dirty = true;
    registry.remove<AnimationMoveTo>(entity);

    // Check if other animations are still playing
    auto& counter = registry.get<AnimationCounter>(entity);

    if (counter.count > 1)
    {
      --counter.count;
    }
    else
    {
      registry.remove<AnimationCounter>(entity);
      // TODO: Move state to target component and set it by the user
      target.value->state = UIComponent::State::Hidden;
      target.value->propagate_state();
    }
  }
}
}  // namespace dl::ui
