#include "./debug_info.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"

namespace dl::ui
{
DebugInfo::DebugInfo() : UIComponent({200, 100})
{
  visible = true;
  position = {30, 30, 0};

  m_container = emplace<Container>(size, 0x1b2420aa);

  m_label = m_container->emplace<Label>("FPS: ");
  m_label->position = {15, 15, 0};

  placement = Placement::Absolute;
}

void DebugInfo::set_content(const std::string& text) { m_label->text.set_text(text); }

}  // namespace dl::ui
