#include "./animation.hpp"

#include <cmath>

namespace dl::ui::animation
{
constexpr double pi = 3.14159265358979323846;

double in_sine(const double t) { return 1 - std::cos((t * pi) / 2.0); }

double out_sine(const double t) { return std::sin((t * pi) / 2.0); }

double in_out_sine(const double t) { return -(std::cos(pi * t) - 1.0) / 2.0; }

double in_quad(const double t) { return t * t; }

double out_quad(const double t) { return 1.0 - (1.0 - t) * (1.0 - t); }

double in_out_quad(const double t) { return t < 0.5 ? 2.0 * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 2) / 2.0; }

double in_cubic(const double t) { return t * t * t; }

double out_cubic(const double t) { return 1.0 - std::pow(1.0 - t, 3.0); }

double in_out_cubic(const double t) { return t < 0.5 ? 4.0 * t * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 3.0) / 2.0; }

double in_quart(const double t) { return t * t * t * t; }

double out_quart(const double t) { return 1.0 - std::pow(1.0 - t, 4.0); }

double in_out_quart(const double t)
{
  return t < 0.5 ? 8.0 * t * t * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 4.0) / 2.0;
}

double in_quint(const double t) { return t * t * t * t * t; }

double out_quint(const double t) { return 1.0 - std::pow(1.0 - t, 5.0); }

double in_out_quint(const double t)
{
  return t < 0.5 ? 16.0 * t * t * t * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 5.0) / 2.0;
}

double in_expo(const double t) { return t == 0.0 ? 0.0 : std::pow(2.0, 10.0 * t - 10.0); }

double out_expo(const double t) { return t == 1.0 ? 1.0 : 1.0 - std::pow(2.0, -10.0 * t); }

double in_out_expo(const double t)
{
  return t == 0.0   ? 0.0
         : t == 1.0 ? 1.0
         : t < 0.5  ? std::pow(2.0, 20.0 * t - 10.) / 2.0
                    : (2.0 - std::pow(2.0, -20.0 * t + 10.0)) / 2.0;
}

double in_circ(const double t) { return 1.0 - std::sqrt(1.0 - std::pow(t, 2.0)); }

double out_circ(const double t) { return std::sqrt(1.0 - std::pow(t - 1.0, 2.0)); }

double in_out_circ(const double t)
{
  return t < 0.5 ? (1.0 - std::sqrt(1.0 - std::pow(2.0 * t, 2.0))) / 2.0
                 : (std::sqrt(1.0 - std::pow(-2.0 * t + 2.0, 2.0)) + 1.0) / 2.0;
}

double in_back(const double t)
{
  const double c1 = 1.70158;
  const double c3 = c1 + 1.0;

  return c3 * t * t * t - c1 * t * t;
}

double out_back(const double t)
{
  const double c1 = 1.70158;
  const double c3 = c1 + 1.0;

  return 1.0 + c3 * std::pow(t - 1.0, 3.0) + c1 * std::pow(t - 1.0, 2.0);
}

double in_out_back(const double t)
{
  const double c1 = 1.70158;
  const double c2 = c1 * 1.525;

  return t < 0.5 ? (std::pow(2.0 * t, 2.0) * ((c2 + 1.0) * 2.0 * t - c2)) / 2.0
                 : (std::pow(2.0 * t - 2.0, 2.0) * ((c2 + 1.0) * (t * 2.0 - 2.0) + c2) + 2.0) / 2.0;
}

double in_elastic(const double t)
{
  const double c4 = (2 * pi) / 3.0;

  return t == 0.0 ? 0.0 : t == 1.0 ? 1.0 : -std::pow(2.0, 10.0 * t - 10.0) * std::sin((t * 10.0 - 10.75) * c4);
}

double out_elastic(const double t)
{
  const double c4 = (2.0 * pi) / 3.0;

  return t == 0.0 ? 0.0 : t == 1.0 ? 1.0 : std::pow(2, -10 * t) * std::sin((t * 10 - 0.75) * c4) + 1;
}

double in_out_elastic(const double t)
{
  const double c5 = (2.0 * pi) / 4.5;

  return t == 0.0   ? 0.0
         : t == 1.0 ? 1.0
         : t < 0.5  ? -(std::pow(2.0, 20.0 * t - 10.0) * std::sin((20.0 * t - 11.125) * c5)) / 2.0
                    : (std::pow(2.0, -20.0 * t + 10.0) * std::sin((20.0 * t - 11.125) * c5)) / 2.0 + 1.0;
}

double out_bounce(const double t)
{
  const double n1 = 7.5625;
  const double d1 = 2.75;

  if (t < 1.0 / d1)
  {
    return n1 * t * t;
  }
  else if (t < 2.0 / d1)
  {
    return n1 * (t - 1.5 / d1) * t + 0.75;
  }
  else if (t < 2.5 / d1)
  {
    return n1 * (t - 2.25 / d1) * t + 0.9375;
  }
  else
  {
    return n1 * (t - 2.625 / d1) * t + 0.984375;
  }
}

double in_bounce(const double t) { return 1.0 - out_bounce(1.0 - t); }

double in_out_bounce(const double t)
{
  return t < 0.5 ? (1.0 - out_bounce(1.0 - 2.0 * t)) / 2.0 : (1.0 + out_bounce(2.0 * t - 1.0)) / 2.0;
}

double get_ease_value(const double t, Easing easing)
{
  switch (easing)
  {
  case Easing::Linear:
    return t;
  case Easing::InSine:
    return in_sine(t);
  case Easing::OutSine:
    return out_sine(t);
  case Easing::InOutSine:
    return in_out_sine(t);
  case Easing::InQuad:
    return in_quad(t);
  case Easing::OutQuad:
    return out_quad(t);
  case Easing::InOutQuad:
    return in_out_quad(t);
  case Easing::InCubic:
    return in_cubic(t);
  case Easing::OutCubic:
    return out_cubic(t);
  case Easing::InOutCubic:
    return in_out_cubic(t);
  case Easing::InQuart:
    return in_quart(t);
  case Easing::OutQuart:
    return out_quart(t);
  case Easing::InOutQuart:
    return in_out_quart(t);
  case Easing::InQuint:
    return in_quint(t);
  case Easing::OutQuint:
    return out_quint(t);
  case Easing::InOutQuint:
    return in_out_quint(t);
  case Easing::InExpo:
    return in_expo(t);
  case Easing::OutExpo:
    return out_expo(t);
  case Easing::InOutExpo:
    return in_out_expo(t);
  case Easing::InCirc:
    return in_circ(t);
  case Easing::OutCirc:
    return out_circ(t);
  case Easing::InOutCirc:
    return in_out_circ(t);
  case Easing::InBack:
    return in_back(t);
  case Easing::OutBack:
    return out_back(t);
  case Easing::InOutBack:
    return in_out_back(t);
  case Easing::InElastic:
    return in_elastic(t);
  case Easing::OutElastic:
    return out_elastic(t);
  case Easing::InOutElastic:
    return in_out_elastic(t);
  case Easing::InBounce:
    return in_bounce(t);
  case Easing::OutBounce:
    return out_bounce(t);
  case Easing::InOutBounce:
    return in_out_bounce(t);
  default:
    return t;
  }
  return t;
}
}  // namespace dl::ui::animation
