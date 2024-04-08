#pragma once

#include <functional>
#include <string>

#include "./component.hpp"
#include "ui/types.hpp"
#include "ui/components/text_button.hpp"
#include "ui/components/button.hpp"
#include "ui/components/container.hpp"
#include "ui/components/scrollable.hpp"
#include "ui/components/label.hpp"

namespace dl::ui
{

template <class T>
class TextButtonList : public UIComponent
{
 public:
  struct Params
  {
    ItemList<T> items{};
    std::function<void(const T&)> on_left_click{};
    std::function<void(const T&)> on_right_click{};
    std::function<void(const T&)> on_hover{};
    Vector2i size{300, 400};
    Vector2i margin{0, 0};
    Vector2i button_size{200, 40};
    uint32_t line_spacing = 0;
  };

  ItemList<T> items{};
  std::function<void(const T&)> on_left_click{};
  std::function<void(const T&)> on_right_click{};
  std::function<void(const T&)> on_hover{};
  Vector2i button_size{200, 40};
  uint32_t line_spacing = 0;
  Scrollable* scrollable = nullptr;
  Container* container = nullptr;

  TextButtonList(UIContext& context, Params params)
    : UIComponent(context, "TextButtonList"),
      items(std::move(params.items)),
      on_left_click(std::move(params.on_left_click)),
      on_right_click(std::move(params.on_right_click)),
      on_hover(std::move(params.on_hover))
  {
    size = std::move(params.size);
    margin = std::move(params.margin);
    button_size = std::move(params.button_size);
    line_spacing = params.line_spacing;

    scrollable = emplace<Scrollable>();
    container = scrollable->emplace<Container>();

    scrollable->size = size;
    container->size = size;

    create_buttons();
  }

  void create_buttons()
  {
    container->children.clear();

    if (items.empty())
    {
      return;
    }

    const auto items_size = items.size();

    container->children.reserve(items_size);

    for (size_t i = 0; i < items_size; ++i)
    {
      const auto& item = items[i];

      auto button = container->emplace<TextButton>(TextButtonParams{
          .text = item.second,
          .on_left_click = [this, &item]()
          {
            if (on_left_click)
            {
              on_left_click(item.first);
            }
          },
          .on_right_click = [this, &item]()
          {
            if (on_right_click)
            {
              on_right_click(item.first);
            }
          },
          .on_hover = [this, &item]()
          {
            if (on_hover)
            {
              on_hover(item.first);
            }
          },
      });

      button->size = Vector2i{button_size.x - 2 * margin.x, button_size.y};
      button->label->x_alignment = XAlignement::Left;
      button->label->y_alignment = YAlignement::Center;
      button->position.x = margin.x;
      button->position.y = i * (button_size.y + line_spacing) + margin.y + i;
      spdlog::debug("Button pos: {} {}", button->position.x, button->position.y);
    }

    const auto height = static_cast<int>(items_size * button_size.y + (items_size - 1) * line_spacing + 2 * margin.y);
    container->size = Vector2i{button_size.x, height};
    spdlog::debug("Container size: {} {}", container->size.x, container->size.y);
    spdlog::debug("Height, items: {} {}", height, items_size);
    spdlog::debug("POS: ({}, {}) ({}, {}) ({}, {})", absolute_position.x, absolute_position.y, container->absolute_position.x, container->absolute_position.y, scrollable->absolute_position.x, scrollable->absolute_position.y);
  }

  void update()
  {
    // spdlog::debug("POS: ({}, {}) ({}, {}) ({}, {})", absolute_position.x, absolute_position.y, container->absolute_position.x, container->absolute_position.y, scrollable->absolute_position.x, scrollable->absolute_position.y);
  }
};

}  // namespace dl::ui
