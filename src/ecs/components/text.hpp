#pragma once

#include "../../graphics/text.hpp"

namespace dl
{
struct StaticText
{
  std::unique_ptr<Text> text;

  StaticText(const std::wstring& string, const std::string& color = "#ffffffff", const size_t font_size = 32)
  {
    text.reset(new Text(string, "font-1980", font_size, color));
  }
};
}  // namespace dl
