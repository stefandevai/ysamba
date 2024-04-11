#include "./gameplay_modals.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/entity.hpp>

#include "ui/compositions/action_menu.hpp"
#include "ui/compositions/item_details.hpp"
#include "ui/compositions/item_selection.hpp"
#include "ui/compositions/selected_inventory.hpp"
#include "ui/compositions/society_inventory.hpp"
#include "ui/ui_manager.hpp"
#include "world/world.hpp"

namespace dl::ui
{
GameplayModals::GameplayModals(UIManager& ui_manager, World& world) : m_ui_manager(ui_manager)
{
  auto on_select_item = [this](entt::entity entity)
  {
    // TODO: Open item details menu
    (void)entity;
    item_details->show();
  };

  selected_inventory = m_ui_manager.emplace<SelectedInventory>(SelectedInventory::Params{
      .on_select = on_select_item,
  });
  society_inventory = m_ui_manager.emplace<SocietyInventory>(SocietyInventory::Params{
      .on_select = on_select_item,
  });
  item_details = m_ui_manager.emplace<ItemDetails>();
  item_selection = m_ui_manager.emplace<ItemSelection>(ItemSelection::Params{
      .world = world,
  });
  action_menu = m_ui_manager.emplace<ActionMenu>(ActionMenu::Params{});
}
}  // namespace dl::ui
