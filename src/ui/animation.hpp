#pragma once

#include <entt/entity/registry.hpp>

namespace dl::ui
{
class UIComponent;

enum class Easing
{
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

struct Animation
{
  UIComponent* component = nullptr;
  double duration = 1.0;
  double time_left = 1.0;
  Easing easing = Easing::InOutCubic;

  Animation(UIComponent* component, const double duration, const Easing easing)
      : component(component), duration(duration), time_left(duration), easing(easing)
  {
  }
};

struct AnimationFadeIn : public Animation
{
  AnimationFadeIn(UIComponent* component, const double duration, const Easing easing)
      : Animation(component, duration, easing)
  {
  }
};

struct AnimationFadeOut : public Animation
{
  AnimationFadeOut(UIComponent* component, const double duration, const Easing easing)
      : Animation(component, duration, easing)
  {
  }
};
}  // namespace dl::ui
