#pragma once

#include <entt/entity/entity.hpp>

#include "./button_list.hpp"
#include "./component.hpp"
#include "./label.hpp"
#include "./scrollable.hpp"
#include "./window_frame.hpp"
#include "core/maths/vector.hpp"
#include "graphics/display.hpp"
#include "ui/types.hpp"
#include "world/metadata.hpp"

namespace dl::ui
{
template <class T = uint32_t>
class ScrollableList : public UIComponent
{
 public:
  uint32_t color = 0x1b2420aa;
  std::string title{};
  ButtonList<T>* list = nullptr;
  Scrollable* scrollable = nullptr;

  ScrollableList(UIContext& context) : UIComponent(context)
  {
    scrollable = emplace<Scrollable>();
    list = scrollable->emplace<ButtonList<T>>();
  }

  void init()
  {
    if (!title.empty() && m_title_label == nullptr)
    {
      m_title_label = emplace<Label>(title);
      // TODO: Remove hardcoded margins
      list->position.y += 16;
    }
    else if (!title.empty() && m_title_label != nullptr)
    {
      m_title_label->set_text(title);
    }
    else if (title.empty() && m_title_label != nullptr)
    {
      scrollable->erase(m_title_label);
      m_title_label = nullptr;
      // TODO: Remove hardcoded margins
      list->position.y -= 16;
    }

    scrollable->size = size;
    list->button_size = Vector2i{size.x, 32};
  }

  void set_items(const ItemList<T>& items) { list->set_items(items); }

  void set_on_select(const std::function<void(const T)>& on_select) { list->set_on_select(on_select); }

  void reset_scroll()
  {
    if (scrollable == nullptr)
    {
      return;
    }

    scrollable->reset_scroll();
  }

 private:
  Label* m_title_label = nullptr;
  std::function<void(const T i)> m_on_select;
  ItemList<T> m_items;
};

}  // namespace dl::ui
