#pragma once

namespace dl::ui
{
class UIManager;
class ItemDetails;
class Inventory;
class SocietyInventory;

class GameplayModals
{
 public:
  // Item details composition for a selected item
  ItemDetails* item_details = nullptr;

  // Inventory UI composition for selected entities only
  Inventory* selected_inventory = nullptr;

  // Inventory UI composition that displays storage items and carried items by all society agents
  SocietyInventory* society_inventory = nullptr;

  GameplayModals(UIManager& ui_manager);

  // Remove copy/move constructors and assignment operators
  GameplayModals(GameplayModals const&) = delete;
  GameplayModals(GameplayModals const&&) = delete;
  void operator=(GameplayModals const&) = delete;
  void operator=(GameplayModals const&&) = delete;

 private:
  UIManager& m_ui_manager;
};

}  // namespace dl::ui
