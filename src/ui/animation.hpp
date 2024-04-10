#pragma once

#include <entt/entity/registry.hpp>

#include "core/maths/vector.hpp"

namespace dl::ui
{
class UIComponent;

enum class Easing
{
  Linear,
  InSine,
  OutSine,
  InOutSine,
  InQuad,
  OutQuad,
  InOutQuad,
  InCubic,
  OutCubic,
  InOutCubic,
  InQuart,
  OutQuart,
  InOutQuart,
  InQuint,
  OutQuint,
  InOutQuint,
  InExpo,
  OutExpo,
  InOutExpo,
  InCirc,
  OutCirc,
  InOutCirc,
  InBack,
  OutBack,
  InOutBack,
  InElastic,
  OutElastic,
  InOutElastic,
  InBounce,
  OutBounce,
  InOutBounce,
};

enum class AnimationMovePosition
{
  Top,
  Bottom,
  Left,
  Right,
};

static const Vector3i AnimationNullPosition = Vector3i{-99999, -99999, -99999};

struct Animation
{
  double duration = 1.0;
  double time_spent = 0.0;
  Easing easing = Easing::Linear;

  Animation(const double duration, const Easing easing) : duration(duration), easing(easing) {}
};

struct AnimationTarget
{
  UIComponent* value = nullptr;
};

struct AnimationCounter
{
  uint32_t count = 1;
};

struct AnimationFadeIn : public Animation
{
  AnimationFadeIn(const double duration, const Easing easing) : Animation(duration, easing) {}
};

struct AnimationFadeOut : public Animation
{
  AnimationFadeOut(const double duration, const Easing easing) : Animation(duration, easing) {}
};

struct AnimationMoveFrom : public Animation
{
  AnimationMovePosition from = AnimationMovePosition::Top;
  Vector3i start_position = AnimationNullPosition;
  Vector3i original_position{};
  Vector3i position_variation{};

  AnimationMoveFrom(const double duration, const Easing easing, AnimationMovePosition from)
      : Animation(duration, easing), from(from)
  {
  }
  AnimationMoveFrom(const double duration, const Easing easing, AnimationMovePosition from, Vector3i start_position)
      : Animation(duration, easing), from(from), start_position(std::move(start_position))
  {
  }
};

struct AnimationMoveTo : public Animation
{
  AnimationMovePosition to = AnimationMovePosition::Top;
  Vector3i end_position = AnimationNullPosition;
  Vector3i original_position{};
  Vector3i position_variation{};

  AnimationMoveTo(const double duration, const Easing easing, AnimationMovePosition to)
      : Animation(duration, easing), to(to)
  {
  }
  AnimationMoveTo(const double duration, const Easing easing, AnimationMovePosition to, Vector3i end_position)
      : Animation(duration, easing), to(to), end_position(std::move(end_position))
  {
  }
};

namespace animation
{
double get_ease_value(double t, Easing easing);
}

}  // namespace dl::ui
