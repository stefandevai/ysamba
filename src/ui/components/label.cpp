#include "./label.hpp"

#include <spdlog/spdlog.h>

#include "core/asset_manager.hpp"
#include "graphics/display.hpp"
#include "graphics/font.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
Label::Label(UIContext& context, const std::string value) : Label(context, Params{.value = std::move(value)}) {}

Label::Label(UIContext& context, Params params)
    : UIComponent(context, "Label"), value(std::move(params.value)), wrap(params.wrap)
{
  is_renderable = true;
  text.font = m_context.asset_manager->get<Font>(text.typeface);
  text.color.set(params.color);

  if (wrap)
  {
    const auto& window_size = Display::get_window_size();
    const auto wrap_width
        = (parent == nullptr || parent->size.x == 0) ? window_size.x - 2 * margin.x : parent->size.x - 2 * margin.x;
    text.set_text_wrapped(value, wrap_width);
  }
  else
  {
    text.set_text(value);
  }

  text.initialize(m_context.renderer->asset_manager);
  size = text.get_size();

  has_initialized = true;
}

void Label::init()
{
  if (wrap)
  {
    const auto& window_size = Display::get_window_size();
    const auto wrap_width
        = (parent == nullptr || parent->size.x == 0) ? window_size.x - 2 * margin.x : parent->size.x - 2 * margin.x;
    text.set_text_wrapped(value, wrap_width);
  }
  else
  {
    text.set_text(value);
  }

  text.initialize(m_context.renderer->asset_manager);
  size = text.get_size();
}

void Label::render()
{
  if (!is_active())
  {
    return;
  }

  if (text.color.opacity_factor != opacity)
  {
    text.color.opacity_factor = opacity;
  }

  m_context.renderer->ui_pass.batch.text(text, absolute_position.x, absolute_position.y, absolute_position.z);
}

void Label::set_text(const std::string value)
{
  if (this->value == value)
  {
    return;
  }

  this->value = std::move(value);
  has_initialized = false;
}

void Label::set_color(uint32_t color)
{
  if (color == text.color.int_color)
  {
    return;
  }

  text.color.set(color);
  has_initialized = false;
}
}  // namespace dl::ui
