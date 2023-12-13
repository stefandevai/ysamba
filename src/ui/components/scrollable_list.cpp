#include "./scrollable_list.hpp"

#include <spdlog/spdlog.h>

#include "./button_list.hpp"
#include "./container.hpp"
#include "./scrollable.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
void ScrollableList::init()
{
  m_container = emplace<Container>(size, color);
  m_scrollable = m_container->emplace<Scrollable>();
  m_scrollable->size = size;
  m_list = m_scrollable->emplace<ButtonList>();
  m_list->button_size = Vector2i{size.x, 45};
  m_list->set_on_select(m_on_select);
  m_list->set_items(m_items);
  m_has_initialized = true;
}

void ScrollableList::set_items(const ItemList& items)
{
  m_items = items;

  if (m_list != nullptr)
  {
    m_list->set_items(items);
  }
}

void ScrollableList::set_on_select(const std::function<void(const uint32_t)>& on_select)
{
  m_on_select = on_select;

  if (m_list != nullptr)
  {
    m_list->set_on_select(on_select);
  }
}

}  // namespace dl::ui
