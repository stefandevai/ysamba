#include "./scrollable_list.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/entity.hpp>

#include "./button_list.hpp"
#include "./label.hpp"
#include "./scrollable.hpp"
#include "./window_frame.hpp"
#include "core/maths/vector.hpp"
#include "graphics/display.hpp"
#include "world/metadata.hpp"

namespace dl::ui
{
template <class T>
ScrollableList<T>::ScrollableList(UIContext& context) : UIComponent(context)
{
  m_scrollable = emplace<Scrollable>();
  m_list = m_scrollable->emplace<ButtonList<T>>();
}

template <class T>
void ScrollableList<T>::init()
{
  if (!title.empty() && m_title_label == nullptr)
  {
    m_title_label = emplace<Label>(title);
    // TODO: Remove hardcoded margins
    m_list->position.y += 16;
  }
  else if (!title.empty() && m_title_label != nullptr)
  {
    m_title_label->set_text(title);
  }
  else if (title.empty() && m_title_label != nullptr)
  {
    m_scrollable->erase(m_title_label);
    m_title_label = nullptr;
    // TODO: Remove hardcoded margins
    m_list->position.y -= 16;
  }

  m_scrollable->size = size;
  m_list->button_size = Vector2i{size.x, 32};
}

template <class T>
void ScrollableList<T>::set_items(const ItemList<T>& items)
{
  m_list->set_items(items);
}

template <class T>
void ScrollableList<T>::set_on_select(const std::function<void(const T)>& on_select)
{
  m_list->set_on_select(on_select);
}

template <class T>
void ScrollableList<T>::reset_scroll()
{
  if (m_scrollable == nullptr)
  {
    return;
  }

  m_scrollable->reset_scroll();
}

template class ScrollableList<uint32_t>;
template class ScrollableList<entt::entity>;
template class ScrollableList<std::pair<entt::entity, entt::entity>>;
template class ScrollableList<WorldMetadata>;
}  // namespace dl::ui
