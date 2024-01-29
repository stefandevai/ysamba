#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"
#include "world/metadata.hpp"

namespace dl::ui
{
template <class T>
class ScrollableList;
class WindowFrame;

class WorldList : public UIComponent
{
 public:
  WorldList(UIContext& context);
  void set_actions(const ItemList<WorldMetadata>& actions);
  void set_on_select(const std::function<void(const WorldMetadata&)>& on_select);
  void show();
  void hide();

 private:
  ScrollableList<WorldMetadata>* m_scrollable_list = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
