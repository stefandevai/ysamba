#pragma once

#include <entt/entity/fwd.hpp>

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class ScrollableTextButtonList;
class WindowFrame;

using EntityPair = std::pair<entt::entity, entt::entity>;

class ItemSelection : public UIComponent
{
 public:
  struct Params
  {
    const ItemList<EntityPair> items{};
    const std::function<void(const EntityPair&)> on_select{};
  };

  ItemSelection(UIContext& context, Params params);

  void process_input();
  void show();
  void hide();
  void set_items(const ItemList<EntityPair> items);
  void set_on_select(const std::function<void(const EntityPair&)> on_select);

 private:
  ScrollableTextButtonList<EntityPair>* m_items = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
