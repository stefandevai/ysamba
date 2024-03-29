#pragma once

#include "ui/components/component.hpp"

namespace dl::ui
{
class Label;
class TextInput;
class Button;

class WorldCreationPanel : public UIComponent
{
 public:
  size_t min_world_name_length = 2;
  size_t max_world_name_length = 24;

  WorldCreationPanel(UIContext& context);

  void on_save(const std::function<void()>& on_save_fn);
  std::string& get_name();
  bool validate();

 private:
  Label* m_label = nullptr;
  TextInput* m_text_input = nullptr;
  Button* m_save_button = nullptr;
};

}  // namespace dl::ui
