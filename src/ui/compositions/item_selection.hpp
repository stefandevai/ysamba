#pragma once

#include <entt/entity/fwd.hpp>

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class ScrollableList;
class WindowFrame;

using EntityPair = std::pair<entt::entity, entt::entity>;

class ItemSelection : public UIComponent
{
 public:
  ItemSelection(UIContext& context, const std::function<void(const EntityPair& i)>& on_select);

  void set_items(const ItemList<EntityPair>& items);
  void show();
  void hide();

 private:
  ScrollableList<EntityPair>* m_items = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
