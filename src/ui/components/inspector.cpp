#include "./inspector.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"

namespace dl::ui
{
Inspector::Inspector() : UIComponent({300, 100})
{
  visible = false;
  position = {30, 30, 0};

  m_container = std::make_shared<Container>(size, 0x1b2420aa);

  m_label = std::make_shared<Label>("");
  m_label->position = {15, 15, 0};

  m_container->children.push_back(m_label);
  children.push_back(m_container);

  placement = Placement::Absolute;
  x_alignment = XAlignement::Right;
}

void Inspector::set_content(const std::string& text)
{
  dirty = true;
  m_label->text.set_text(text);
}

}  // namespace dl::ui
