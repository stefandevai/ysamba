#pragma once

namespace dl::ui
{
template <typename... Ts>
struct AnimationOverload : Ts...
{
  using Ts::operator()...;
};
template <class... Ts>
AnimationOverload(Ts...) -> AnimationOverload<Ts...>;

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
  Easing easing = Easing::InOutCubic;
  double duration = 1.0;
  double time_left = 1.0;
};

struct FadeOutAnimation
{
  Easing easing = Easing::InOutCubic;
  double duration = 1.0;
  double time_left = 1.0;
};

using Animation = std::variant<FadeInAnimation, FadeOutAnimation>;
}  // namespace dl::ui
