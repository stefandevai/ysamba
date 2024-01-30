#include "./world_creation_panel.hpp"

#include <spdlog/spdlog.h>

#include "ui/animation.hpp"
#include "ui/components/button.hpp"
#include "ui/components/label.hpp"
#include "ui/components/text_input.hpp"

namespace dl::ui
{
WorldCreationPanel::WorldCreationPanel(UIContext& context) : UIComponent(context)
{
  size = {250, 100};
  position = {30, 30, 0};
  placement = Placement::Absolute;
  x_alignment = XAlignement::Right;

  m_label = emplace<Label>("Name:");
  m_label->position = {20, 20, 0};

  m_text_input = emplace<TextInput>();
  m_text_input->size.x = 250;
  m_text_input->position = {20, 40, 0};
  m_text_input->placeholder = "Enter world name";

  m_save_button = emplace<Button>();
  m_save_button->size.x = 250;
  m_save_button->position = {20, 80, 0};
  m_save_button->text = "Save";
}

std::string& WorldCreationPanel::get_name() { return m_text_input->text; }

void WorldCreationPanel::on_save(const std::function<void()>& on_save_fn) { m_save_button->on_click = on_save_fn; }

bool WorldCreationPanel::validate()
{
  const size_t name_length = m_text_input->text.size();

  return name_length > min_world_name_length && name_length < max_world_name_length;
}

}  // namespace dl::ui
