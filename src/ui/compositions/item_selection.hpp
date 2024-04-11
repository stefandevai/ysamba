#pragma once

#include <entt/entity/fwd.hpp>

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl
{
class World;
}

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
    World& world;
    const ItemList<EntityPair> items{};
    const std::function<void(const EntityPair&)> on_select{};
  };

  ItemSelection(UIContext& context, Params params);

  void process_input();
  void show();
  void hide();
  void set_items(const ItemList<EntityPair> items);
  void set_on_select(const std::function<void(const EntityPair&)> on_select);
  void set_items_from_entity(entt::registry& registry, entt::entity entity);

 private:
  World& m_world;
  ScrollableTextButtonList<EntityPair>* m_items = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
