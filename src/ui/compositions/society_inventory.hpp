#pragma once

#include <entt/entity/entity.hpp>

#include "ui/components/component.hpp"
#include "ui/types.hpp"

namespace dl::ui
{
template <class T>
class ScrollableTextButtonList;
class WindowFrame;

class SocietyInventory : public UIComponent
{
 public:
  struct Params
  {
    const ItemList<entt::entity> items{};
    const std::function<void(entt::entity)> on_select{};
  };

  SocietyInventory(UIContext& context, Params params);

  void process_input();
  void set_items(const ItemList<entt::entity>& items);
  void show();
  void hide();

 private:
  ScrollableTextButtonList<entt::entity>* m_items = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
