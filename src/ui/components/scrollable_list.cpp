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
  scrollable = emplace<Scrollable>();
  list = scrollable->emplace<ButtonList<T>>();
}

template <class T>
void ScrollableList<T>::init()
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

template <class T>
void ScrollableList<T>::set_items(const ItemList<T>& items)
{
  list->set_items(items);
}

template <class T>
void ScrollableList<T>::set_on_select(const std::function<void(const T)>& on_select)
{
  list->set_on_select(on_select);
}

template <class T>
void ScrollableList<T>::reset_scroll()
{
  if (scrollable == nullptr)
  {
    return;
  }

  scrollable->reset_scroll();
}

template class ScrollableList<uint32_t>;
template class ScrollableList<entt::entity>;
template class ScrollableList<std::pair<entt::entity, entt::entity>>;
template class ScrollableList<WorldMetadata>;
}  // namespace dl::ui
