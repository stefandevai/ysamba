#include "./color.hpp"

#include <spdlog/spdlog.h>

#include <cstdio>
#include <sstream>

namespace dl
{
Color::Color(const uint32_t color) : int_color(color), rgba_color(Color::int_to_rgba(color)) {}

Color::Color(std::string_view color) : int_color(Color::string_to_int(color)), rgba_color(Color::int_to_rgba(int_color))
{
}

Color::Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    : int_color(Color::rgba_to_int(r, g, b, a)), rgba_color(RGBA{r, g, b, a})
{
}

void Color::set(const uint32_t color)
{
  int_color = color;
  rgba_color = int_to_rgba(color);
}

void Color::set(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
  int_color = rgba_to_int(r, g, b, a);
  rgba_color.r = r;
  rgba_color.g = g;
  rgba_color.b = b;
  rgba_color.a = a;
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

uint32_t Color::string_to_int(std::string_view color)
{
  uint32_t parsed_color = 0;

  if (color.size() == 9)
  {
    std::stringstream ss;
    std::string hex_str{color.substr(1)};
    ss << std::hex << hex_str;
    ss >> parsed_color;
  }
  else if (color.size() == 8)
  {
    std::stringstream ss;
    ss << std::hex << color;
    ss >> parsed_color;
  }
  else if (color.size() == 6)
  {
    std::stringstream ss;
    ss << std::hex << color;
    ss >> parsed_color;
    parsed_color = (parsed_color << 8) | 0x000000FF;
  }

  return parsed_color;
}

}  // namespace dl
