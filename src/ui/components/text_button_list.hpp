#pragma once

#include <fmt/format.h>

#include <functional>

#include "./component.hpp"
#include "ui/components/container.hpp"
#include "ui/components/label.hpp"
#include "ui/components/mouse_region.hpp"
#include "ui/components/text_button.hpp"
#include "ui/types.hpp"
#include "ui/utils.hpp"

namespace dl::ui
{
enum class EnumerationType
{
  None,
  Alphabetical,
  Numerical,
};

template <class T>
class TextButtonList : public UIComponent
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
    int line_spacing = 10;
    Vector2i button_size{0, 0};
    Vector2i button_margin{0, 0};
    uint32_t button_text_color = 0xCEE2DFFF;
    uint32_t button_text_hover_color = 0xA1B9B6FF;
    bool enumerate = false;
    EnumerationType enumeration_type = EnumerationType::Alphabetical;
  };

  ItemList<T> items{};
  std::function<void(const T&)> on_left_click{};
  std::function<void(const T&)> on_right_click{};
  std::function<void(const T&)> on_mouse_over{};
  std::function<void(const T&)> on_mouse_out{};
  int line_spacing{};
  Vector2i button_size{};
  Vector2i button_margin{};
  uint32_t button_text_color{};
  uint32_t button_text_hover_color{};
  bool enumerate{};
  EnumerationType enumeration_type{};
  Container* container = nullptr;
  std::vector<TextButton*> buttons{};

  TextButtonList(UIContext& context, Params params)
      : UIComponent(context, "TextButtonList"),
        items(std::move(params.items)),
        on_left_click(std::move(params.on_left_click)),
        on_right_click(std::move(params.on_right_click)),
        on_mouse_over(std::move(params.on_mouse_over)),
        on_mouse_out(std::move(params.on_mouse_out)),
        line_spacing(params.line_spacing),
        button_text_color(params.button_text_color),
        button_text_hover_color(params.button_text_hover_color),
        enumerate(params.enumerate),
        enumeration_type(params.enumeration_type)
  {
    size = std::move(params.size);

    if (params.button_size.x != 0 && params.button_size.y != 0)
    {
      button_size = std::move(params.button_size);
    }
    else if (params.button_size.x == 0 && params.button_size.y != 0)
    {
      button_size.x = size.x;
      button_size.y = params.button_size.y;
    }
    else
    {
      button_size.x = size.x;
      button_size.y = 20;
    }

    container = emplace<Container>(Container::Params{
        .size = size,
    });

    create_buttons();
  }

  void process_input()
  {
    if (!enumerate)
    {
      return;
    }

    switch (enumeration_type)
    {
    case EnumerationType::Alphabetical:
    {
      const int index = m_input_manager.get_letter_key_down_index();

      if (index < 0 || static_cast<uint32_t>(index) >= buttons.size())
      {
        return;
      }

      buttons[index]->mouse_region->on_left_click();
      break;
    }
    case EnumerationType::Numerical:
    {
      const int number = m_input_manager.get_number_key_down();

      if (number < 0 || static_cast<uint32_t>(number) >= buttons.size() - 1)
      {
        return;
      }
      else if (number == 0 && buttons.size() >= 10)
      {
        buttons[9]->mouse_region->on_left_click();
        break;
      }
      buttons[number - 1]->mouse_region->on_left_click();
      break;
    }
    default:
      break;
    }
  }

  void create_buttons()
  {
    container->children.clear();
    buttons.clear();

    if (items.empty())
    {
      return;
    }

    const auto items_size = items.size();

    container->children.reserve(items_size);
    buttons.reserve(items_size);

    for (uint32_t i = 0; i < items_size; ++i)
    {
      const auto& item = items[i];

      auto button = container->emplace<TextButton>(TextButton::Params{
          .text = enumerate ? m_get_enumerated_content(item.second, i, enumeration_type) : item.second,
          .on_left_click =
              [this, &item]()
          {
            if (on_left_click)
            {
              on_left_click(item.first);
            }
          },
          .on_right_click =
              [this, &item]()
          {
            if (on_right_click)
            {
              on_right_click(item.first);
            }
          },
          .on_mouse_over =
              [this, &item]()
          {
            if (on_mouse_over)
            {
              on_mouse_over(item.first);
            }
          },
          .on_mouse_out =
              [this, &item]()
          {
            if (on_mouse_out)
            {
              on_mouse_out(item.first);
            }
          },
          .text_color = button_text_color,
          .text_hover_color = button_text_hover_color,
      });

      button->size = Vector2i{button_size.x - 2 * button_margin.x, button_size.y};
      button->label->x_alignment = XAlignement::Left;
      button->label->y_alignment = YAlignement::Center;
      button->position.x = button_margin.x;
      button->position.y = i * (button_size.y + line_spacing) + button_margin.y + i;
      buttons.push_back(button);
    }

    const auto height
        = static_cast<int>(items_size * button_size.y + (items_size - 1) * line_spacing + 2 * button_margin.y);

    size = Vector2i{button_size.x, height};
    container->set_size(size);
  }

 private:
  std::string m_get_enumerated_content(const std::string_view item, const uint32_t index, const EnumerationType type)
  {
    switch (type)
    {
    case EnumerationType::Alphabetical:
    {
      if (index < 52)
      {
        return fmt::format("[#687c82]\\[{}\\][/#] {}", get_character_by_index(index), item);
      }
      break;
    }
    case EnumerationType::Numerical:
    {
      if (index < 9)
      {
        return fmt::format("[#687c82]\\[{}\\][/#] {}", index + 1, item);
      }
      else if (index == 9)
      {
        return fmt::format("[#687c82]\\[{}\\][/#] {}", 0, item);
      }
      break;
    }
    default:
      return std::string(item);
    }

    return std::string(item);
  }
};

}  // namespace dl::ui
