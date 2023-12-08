#include "./color.hpp"

namespace dl
{
Color::Color(const uint32_t color)
    : int_color(color),
      rgba_color(RGBA{uint8_t((int_color & 0xFF000000) >> 24),
                      uint8_t((int_color & 0x00FF0000) >> 16),
                      uint8_t((int_color & 0x0000FF00) >> 8),
                      uint8_t((int_color & 0x000000FF))})
{
}

Color::Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    : int_color(a << 24 | b << 16 | g << 8 | r), rgba_color(RGBA{r, g, b, a})
{
}
}  // namespace dl
