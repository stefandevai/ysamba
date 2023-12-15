#include "./debug_info.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/container.hpp"
#include "ui/components/label.hpp"

namespace dl::ui
{
DebugInfo::DebugInfo(UIContext& context) : UIComponent(context)
{
  size = {200, 100};
  position = {30, 30, 0};

  m_container = emplace<Container>(size, 0x1b242066);

  m_label = m_container->emplace<Label>("FPS: ");
  m_label->position = {15, 15, 0};
  m_label->wrap = false;

  placement = Placement::Absolute;
}

void DebugInfo::set_content(const std::string_view text) { m_label->set_text(text); }

}  // namespace dl::ui
