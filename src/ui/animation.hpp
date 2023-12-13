#pragma once

namespace dl::ui
{
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

struct FadeInAnimation
{
  double duration = 0.5;
  Easing easing = Easing::InOutCubic;
};

struct FadeOutAnimation
{
  double duration = 0.5;
  Easing easing = Easing::InOutCubic;
};

using Animation = std::variant<FadeInAnimation, FadeOutAnimation>;
}  // namespace dl::ui
