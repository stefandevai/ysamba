#include "./world_creation_panel.hpp"

#include <spdlog/spdlog.h>

#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/text_input.hpp"

namespace dl::ui
{
WorldCreationPanel::WorldCreationPanel(UIContext& context) : UIComponent(context)
{
  size = {250, 100};
  position = {30, 30, 0};

  m_label = emplace<Label>("Name:");
  m_label->position = {20, 20, 0};

  m_text_input = emplace<TextInput>();
  m_text_input->size = size;
  m_text_input->position = {20, 40, 0};
  m_text_input->placeholder = "Enter world name";

  placement = Placement::Absolute;
  x_alignment = XAlignement::Right;
}

std::string& WorldCreationPanel::get_name() { return m_text_input->text; }

bool WorldCreationPanel::validate()
{
  const size_t name_length = m_text_input->text.size();

  return name_length > min_world_name_length && name_length < max_world_name_length;
}

}  // namespace dl::ui
