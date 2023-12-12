#include "./scrollable_list.hpp"

#include <spdlog/spdlog.h>

#include "./button_list.hpp"
#include "./container.hpp"
#include "./scrollable.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"
#include "ui/style/list.hpp"

namespace dl::ui
{
ScrollableList::ScrollableList(const ItemList& items,
                               const Vector2i& size,
                               const std::function<void(const uint32_t)>& on_select)
    : UIComponent(size), m_on_select(on_select)
{
  m_container = emplace<Container>(size, 0x1b2420aa);
  m_scrollable = m_container->emplace<Scrollable>();
  m_scrollable->size = size;

  set_items(items);
}

ScrollableList::ScrollableList(const Vector2i& size, const std::function<void(const uint32_t)>& on_select)
    : UIComponent(size), m_on_select(on_select)
{
  m_container = emplace<Container>(size, 0x1b2420aa);
  m_scrollable = m_container->emplace<Scrollable>();
  m_scrollable->size = size;
}

void ScrollableList::set_items(const ItemList& items)
{
  m_scrollable->children.clear();
  m_list = m_scrollable->emplace<ButtonList>(items, m_on_select);
  m_list->button_size = Vector2i{size.x, 45};
}

void ScrollableList::set_on_select(const std::function<void(const uint32_t)>& on_select)
{
  m_list->set_on_select(on_select);
}

}  // namespace dl::ui
