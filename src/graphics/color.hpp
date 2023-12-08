#pragma once

#include <cstdint>

namespace dl
{
struct Color
{
 private:
  struct RGBA
  {
    uint8_t r, g, b, a;
  };

 public:
  Color(const uint32_t color);
  Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255);

  uint32_t int_color = 0xFFFFFFFF;
  RGBA rgba_color{255, 255, 255, 255};
};
}  // namespace dl
