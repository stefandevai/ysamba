#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/container.hpp"
#include "ui/components/label.hpp"

namespace dl::ui
{
Inspector::Inspector() : UIComponent({300, 100})
{
  state = UIComponent::State::Hidden;
  position = {30, 30, 0};

  m_container = emplace<Container>(size, 0x1b2420aa);

  m_label = m_container->emplace<Label>("");
  m_label->position = {15, 15, 0};

  placement = Placement::Absolute;
  x_alignment = XAlignement::Right;
}

void Inspector::set_content(const std::string& text)
{
  dirty = true;
  m_label->text.set_text(text);
}

}  // namespace dl::ui
