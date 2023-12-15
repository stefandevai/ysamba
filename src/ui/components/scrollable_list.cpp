#include "./scrollable_list.hpp"

#include <spdlog/spdlog.h>

#include "./button_list.hpp"
#include "./scrollable.hpp"
#include "./window_frame.hpp"
#include "core/asset_manager.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"

namespace dl::ui
{
ScrollableList::ScrollableList(UIContext& context) : UIComponent(context)
{
  m_scrollable = emplace<Scrollable>();
  m_list = m_scrollable->emplace<ButtonList>();
}

void ScrollableList::init()
{
  m_scrollable->size = size;
  m_list->button_size = Vector2i{size.x, 32};
}

void ScrollableList::set_items(const ItemList& items) { m_list->set_items(items); }

void ScrollableList::set_on_select(const std::function<void(const uint32_t)>& on_select)
{
  m_list->set_on_select(on_select);
}

void ScrollableList::reset_scroll()
{
  if (m_scrollable == nullptr)
  {
    return;
  }

  m_scrollable->reset_scroll();
}

}  // namespace dl::ui
