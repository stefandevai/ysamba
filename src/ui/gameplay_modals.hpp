#pragma once

namespace dl
{
class World;
}

namespace dl::ui
{
class UIManager;
class ItemDetails;
class SelectedInventory;
class SocietyInventory;
class ItemSelection;
class ActionMenu;

class GameplayModals
{
 public:
  // Item details composition for a selected item
  ItemDetails* item_details = nullptr;

  // Inventory UI composition for selected entities only
  SelectedInventory* selected_inventory = nullptr;

  // Inventory UI composition that displays storage items and carried items by all society agents
  SocietyInventory* society_inventory = nullptr;

  // Generic item list that allows selecting items
  ItemSelection* item_selection = nullptr;

  // Displays a list of actions that can be performed by an society agent in a determined context
  ActionMenu* action_menu = nullptr;

  GameplayModals(UIManager& ui_manager, World& world);

  // Remove copy/move constructors and assignment operators
  GameplayModals(GameplayModals const&) = delete;
  GameplayModals(GameplayModals const&&) = delete;
  void operator=(GameplayModals const&) = delete;
  void operator=(GameplayModals const&&) = delete;

 private:
  UIManager& m_ui_manager;
};

}  // namespace dl::ui
