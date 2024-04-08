#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"
#include "world/metadata.hpp"

namespace dl::ui
{
template <class T>
class TextButtonList;
class WindowFrame;

class WorldList : public UIComponent
{
 public:
  WorldList(UIContext& context);
  void process_input();
  void show();
  void hide();
  void set_actions(const ItemList<WorldMetadata>& actions);
  void set_on_select(const std::function<void(const WorldMetadata&)>& on_select);

 private:
  TextButtonList<WorldMetadata>* m_list = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
