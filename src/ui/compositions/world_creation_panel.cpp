#include "./world_creation_panel.hpp"

#include <spdlog/spdlog.h>

#include <i18n_keyval/i18n.hpp>

#include "ui/animation.hpp"
#include "ui/components/label.hpp"
#include "ui/components/mouse_region.hpp"
#include "ui/components/sprite_button.hpp"
#include "ui/components/text_input.hpp"

namespace dl::ui
{
WorldCreationPanel::WorldCreationPanel(UIContext& context, Params params) : UIComponent(context, "WorldCreationPanel")
{
  using namespace i18n::literals;

  size = Vector2i{250, 100};
  position = Vector3i{15, 15, 0};
  placement = Placement::Absolute;
  x_alignment = XAlignement::Right;

  m_label = emplace<Label>("name_label"_t);
  m_label->position = Vector3i{0, 0, 0};

  m_text_input = emplace<TextInput>(TextInput::Params{
      .placeholder = "enter_world_name"_t,
      .size = Vector2i{250, 36},
  });
  m_text_input->size.x = 250;
  m_text_input->position = Vector3i{0, 20, 0};

  m_save_button = emplace<SpriteButton>(SpriteButton::Params{
      .text = "save"_t,
      .size = Vector2i{250, 32},
      .on_left_click = std::move(params.on_save),
  });

  m_save_button->position = Vector3i{0, 64, 0};
}

std::string& WorldCreationPanel::get_name()
{
  return m_text_input->text;
}

void WorldCreationPanel::set_on_save(const std::function<void()> on_save)
{
  m_save_button->mouse_region->on_left_click = std::move(on_save);
}

bool WorldCreationPanel::validate()
{
  const size_t name_length = m_text_input->text.size();

  return name_length > min_world_name_length && name_length < max_world_name_length;
}

}  // namespace dl::ui
