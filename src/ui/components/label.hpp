#pragma once

#include "./component.hpp"
#include "graphics/text.hpp"

namespace dl::ui
{
class Label : public UIComponent
{
 public:
  struct Params
  {
    std::string value{};
    bool wrap = true;
    uint32_t color = 0xFFFFFFFF;
  };

  Text text{};
  std::string value{};
  bool wrap = true;

  Label(UIContext& context, const std::string value);
  Label(UIContext& context, Params params);

  void init();
  void render();

  void set_text(const std::string_view value);
  void set_color(uint32_t color);
};

}  // namespace dl::ui
