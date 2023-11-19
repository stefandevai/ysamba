#pragma once

#include <string>

namespace dl
{
struct Color
{
 private:
  struct rgba_color_info
  {
    int r, g, b, a;
  };

 public:
  Color() {}
  Color(const std::string& color);
  Color(const unsigned r, const unsigned g, const unsigned b, const unsigned a = 255);

  std::string hex_color = "#ffffff";
  unsigned int_color = 4294967295;
  rgba_color_info rgba_color{255, 255, 255, 255};
};
}  // namespace dl
