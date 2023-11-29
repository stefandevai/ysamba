#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
Inspector::Inspector() : UIComponent()
{
  position = {30, 30, 0};
  size = {300, 100};

  m_container = std::make_shared<Container>(size, "#1b2420aa");

  m_label = std::make_shared<Label>("");
  m_label->position = {15, 15, 0};

  m_container->children.push_back(m_label);
  children.push_back(m_container);

  placement = Placement::Absolute;
  x_alignment = XAlignement::Right;
}

void Inspector::set_content(const std::string& text) { m_label->text.set_text(text); }

}  // namespace dl::ui
