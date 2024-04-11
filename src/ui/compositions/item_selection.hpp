#pragma once

#include <entt/entity/registry.hpp>

#include "ecs/utils.hpp"
#include "ui/components/component.hpp"
#include "ui/components/scrollable_text_button_list.hpp"
#include "ui/types.hpp"
#include "world/item_factory.hpp"
#include "world/world.hpp"

namespace dl
{
class World;
}

namespace dl::ui
{
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
    const std::string title = "Select Item";
  };

  ItemSelection(UIContext& context, Params params);

  void process_input();
  void show();
  void hide();
  void set_items(const ItemList<EntityPair> items);
  void set_on_select(const std::function<void(const EntityPair&)> on_select);
  void set_items_from_entity(entt::registry& registry, entt::entity entity);
  void set_title(const std::string title);

  template <typename... T>
  void set_items_from_entity_with_components(entt::registry& registry, entt::entity entity)
  {
    auto& all_items = m_items->list->items;

    all_items.clear();

    const auto entity_items = utils::get_entity_items(registry, entity);

    for (const auto item_entity : entity_items)
    {
      if (!registry.all_of<T...>(item_entity))
      {
        continue;
      }

      all_items.push_back({{entity, item_entity}, item_factory::get_item_label(m_world, registry, item_entity)});
    }

    m_items->list->create_buttons();
  }

 private:
  World& m_world;
  ScrollableTextButtonList<EntityPair>* m_items = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
