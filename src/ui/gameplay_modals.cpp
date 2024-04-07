#include "./gameplay_modals.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/entity.hpp>

#include "ui/compositions/inventory.hpp"
#include "ui/compositions/item_details.hpp"
#include "ui/compositions/society_inventory.hpp"
#include "ui/ui_manager.hpp"

namespace dl::ui
{
GameplayModals::GameplayModals(UIManager& ui_manager) : m_ui_manager(ui_manager)
{
  item_details = m_ui_manager.emplace<ItemDetails>();

  auto on_select_item = [this](entt::entity entity)
  {
    // TODO: Open item details menu
    (void)entity;
    item_details->show();
  };

  selected_inventory = m_ui_manager.emplace<Inventory>(on_select_item);
  society_inventory = m_ui_manager.emplace<SocietyInventory>(on_select_item);
}
}  // namespace dl::ui
