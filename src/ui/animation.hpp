#pragma once

#include <entt/entity/registry.hpp>

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

struct AnimationTarget
{
  UIComponent* value = nullptr;
};

struct Animation
{
  double duration = 1.0;
  double time_spent = 0.0;
  Easing easing = Easing::Linear;

  Animation(const double duration, const Easing easing) : duration(duration), easing(easing) {}
};

struct AnimationFadeIn : public Animation
{
  AnimationFadeIn(const double duration, const Easing easing) : Animation(duration, easing) {}
};

struct AnimationFadeOut : public Animation
{
  AnimationFadeOut(const double duration, const Easing easing) : Animation(duration, easing) {}
};

namespace animation
{
double get_ease_value(const double t, Easing easing);
}

}  // namespace dl::ui
