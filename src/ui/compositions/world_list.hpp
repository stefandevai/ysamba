#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"
#include "world/metadata.hpp"

namespace dl::ui
{
template <class T>
class ScrollableTextButtonList;
class WindowFrame;

class WorldList : public UIComponent
{
 public:
  struct Params
  {
    ItemList<WorldMetadata> items{};
    std::function<void(const WorldMetadata&)> on_select{};
  };

  WorldList(UIContext& context, Params params);
  void process_input();
  void show();
  void hide();
  void set_items(const ItemList<WorldMetadata> items);
  void set_on_select(const std::function<void(const WorldMetadata&)>& on_select);

 private:
  ScrollableTextButtonList<WorldMetadata>* m_list = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
