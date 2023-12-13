#include "./color.hpp"

#include <cstdio>

namespace dl
{
Color::Color(const uint32_t color) : int_color(color), rgba_color(Color::int_to_rgba(color)) {}

Color::Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    : int_color(Color::rgba_to_int(r, g, b, a)), rgba_color(RGBA{r, g, b, a})
{
}

uint32_t Color::rgba_to_int(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
  return (r << 24 | g << 16 | b << 8 | a);
}

Color::RGBA Color::int_to_rgba(const uint32_t color)
{
  return RGBA{uint8_t((color & 0xFF000000) >> 24),
              uint8_t((color & 0x00FF0000) >> 16),
              uint8_t((color & 0x0000FF00) >> 8),
              uint8_t((color & 0x000000FF))};
}

}  // namespace dl
