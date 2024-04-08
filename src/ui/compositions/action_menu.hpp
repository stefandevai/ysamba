#pragma once

#include "ui/components/component.hpp"
#include "ui/types.hpp"
#include "world/society/job_type.hpp"

namespace dl::ui
{
template <class T>
class ScrollableTextButtonList;
class WindowFrame;

class ActionMenu : public UIComponent
{
 public:
  ActionMenu(UIContext& context, const ItemList<JobType>& items, const std::function<void(const JobType)>& on_select);
  void set_actions(const ItemList<JobType>& actions);
  void set_on_select(const std::function<void(const JobType)>& on_select);
  void show();
  void hide();

 private:
  ScrollableTextButtonList<JobType>* m_list = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
