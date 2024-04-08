#pragma once

#include <functional>
#include <string>

#include "./component.hpp"
#include "ui/components/label.hpp"
#include "ui/components/scrollable.hpp"
#include "ui/components/text_button_list.hpp"
#include "ui/types.hpp"

namespace dl::ui
{

template <class T>
class ScrollableTextButtonList : public UIComponent
{
 public:
  struct Params
  {
    ItemList<T> items{};
    std::function<void(const T&)> on_left_click{};
    std::function<void(const T&)> on_right_click{};
    std::function<void(const T&)> on_mouse_over{};
    std::function<void(const T&)> on_mouse_out{};
    Vector2i size{300, 485};
    Vector2i margin{10, 10};
    int line_spacing = 10;
    Vector2i button_size{0, 0};
    Vector2i button_margin{0, 0};
    uint32_t button_text_color = 0xCEE2DFFF;
    uint32_t button_text_hover_color = 0xA1B9B6FF;
    uint32_t title_color = 0xC5B75BFF;
    std::string title{};
  };

  uint32_t title_color{};
  std::string title{};
  Scrollable* scrollable = nullptr;
  TextButtonList<T>* list = nullptr;

  ScrollableTextButtonList(UIContext& context, Params params)
      : UIComponent(context, "ScrollableTextButtonList"),
        title_color(params.title_color),
        title(std::move(params.title))
  {
    size = params.size - params.margin * 2;
    margin = std::move(params.margin);

    scrollable = emplace<Scrollable>();
    scrollable->size = size;

    if (!title.empty())
    {
      emplace<Label>(Label::Params{
          .value = title,
          .color = title_color,
      });

      // TODO: Calculate the height of the title label on initialization
      size.y -= 16 + params.line_spacing;
      scrollable->position.y += 16 + params.line_spacing;
      scrollable->size.y -= 16 + params.line_spacing;
    }

    list = scrollable->emplace<TextButtonList<T>>(typename TextButtonList<T>::Params{
        .size = size,
        .items = std::move(params.items),
        .on_left_click = std::move(params.on_left_click),
        .on_right_click = std::move(params.on_right_click),
        .on_mouse_over = std::move(params.on_mouse_over),
        .on_mouse_out = std::move(params.on_mouse_out),
        .line_spacing = params.line_spacing,
        .button_size = std::move(params.button_size),
        .button_margin = std::move(params.button_margin),
        .button_text_color = params.button_text_color,
        .button_text_hover_color = params.button_text_hover_color,
    });
  }
};

}  // namespace dl::ui
