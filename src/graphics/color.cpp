#include "./color.hpp"

namespace dl
{
Color::Color(const uint32_t color) : int_color(color)
{
  rgba_color.r = (int_color & 0xFF000000) >> 24;
  rgba_color.g = (int_color & 0x00FF0000) >> 16;
  rgba_color.b = (int_color & 0x0000FF00) >> 8;
  rgba_color.a = (int_color & 0x000000FF);
}

Color::Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) : rgba_color(RGBA{r, g, b, a})
{
  int_color = a << 24 | b << 16 | g << 8 | r;
}
}  // namespace dl
