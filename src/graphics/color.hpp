#pragma once

#include <cstdint>

namespace dl
{
struct Color
{
 public:
  struct RGBA
  {
    uint8_t r, g, b, a;
  };

  uint32_t int_color = 0xFFFFFFFF;
  RGBA rgba_color{255, 255, 255, 255};

  // How much the real color's opacity will be multiplied
  double opacity_factor = 1.0;

  Color(const uint32_t color);
  Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255);

  static uint32_t rgba_to_int(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255);
  static RGBA int_to_rgba(const uint32_t color);
};
}  // namespace dl
