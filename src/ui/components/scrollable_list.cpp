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
ScrollableList::ScrollableList(const std::vector<std::string>& items,
                               const Vector2i& size,
                               const std::function<void(const int i)>& on_select)
    : UIComponent(), m_on_select(on_select)
{
  this->size = size;

  /* ListStyle style{{15, 15}, 5}; */

  /* m_list = std::make_shared<ButtonList>(items, Vector2i{size.x, 45}, on_select, style); */

  m_scrollable = std::make_shared<Scrollable>();
  m_scrollable->size = size;
  /* m_scrollable->children.push_back(m_list); */

  set_items(items);

  m_container = std::make_shared<Container>(size, "#1b2420aa");
  m_container->children.push_back(m_scrollable);

  children.push_back(m_container);
}

ScrollableList::ScrollableList(const Vector2i& size, const std::function<void(const int i)>& on_select)
    : UIComponent(), m_on_select(on_select)
{
  this->size = size;

  m_scrollable = std::make_shared<Scrollable>();
  m_scrollable->size = size;

  m_container = std::make_shared<Container>(size, "#1b2420aa");
  m_container->children.push_back(m_scrollable);

  children.push_back(m_container);
}

void ScrollableList::set_items(const std::vector<std::string>& items)
{
  ListStyle style{{15, 15}, 5};
  m_list = std::make_shared<ButtonList>(items, Vector2i{size.x, 45}, m_on_select, style);
  m_scrollable->children.clear();
  m_scrollable->children.push_back(m_list);
}

}  // namespace dl::ui
