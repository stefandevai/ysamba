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
  struct Params
  {
    const ItemList<JobType> actions{};
    const std::function<void(JobType)> on_select{};
  };

  ActionMenu(UIContext& context, Params params);
  void process_input();
  void show();
  void hide();
  void set_actions(const ItemList<JobType> actions);
  void set_on_select(const std::function<void(JobType)> on_select);

 private:
  ScrollableTextButtonList<JobType>* m_list = nullptr;
  WindowFrame* m_window_frame = nullptr;
};

}  // namespace dl::ui
