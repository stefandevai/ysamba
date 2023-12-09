#include "./debug_info.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"

namespace dl::ui
{
DebugInfo::DebugInfo() : UIComponent()
{
  position = {30, 30, 0};
  size = {200, 100};

  m_container = std::make_shared<Container>(size, 0x1b2420aa);

  m_label = std::make_shared<Label>("FPS: ");
  m_label->position = {15, 15, 0};

  m_container->children.push_back(m_label);
  children.push_back(m_container);

  placement = Placement::Absolute;
}

void DebugInfo::set_content(const std::string& text) { m_label->text.set_text(text); }

}  // namespace dl::ui
