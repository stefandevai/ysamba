#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class ScrollableTextButtonList;
class WindowFrame;

class SelectedInventory : public UIComponent
{
 public:
  SelectedInventory(UIContext& context, const std::function<void(entt::entity)>& on_select);

  void process_input();
  void set_weared_items(const ItemList<entt::entity>& items);
  void set_carried_items(const ItemList<entt::entity>& items);
  void show();
  void hide();

 private:
  ScrollableTextButtonList<entt::entity>* m_weared_items = nullptr;
  ScrollableTextButtonList<entt::entity>* m_carried_items = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
