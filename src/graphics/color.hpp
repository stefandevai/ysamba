#pragma once

#include <cstdint>
#include <string>

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
  Color(std::string_view color);
  Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255);

  void set(const uint32_t color);
  void set(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255);

  static uint32_t rgba_to_int(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255);
  static RGBA int_to_rgba(const uint32_t color);
  static uint32_t string_to_int(std::string_view color);
};
}  // namespace dl
