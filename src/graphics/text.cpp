#include "./text.hpp"

#include <spdlog/spdlog.h>

#include "./font.hpp"
#include "./sprite.hpp"
#include "core/asset_manager.hpp"

namespace dl
{
Text::Text(const std::string text,
           const std::string font_name,
           const unsigned int font_size,
           const std::string& color,
           const bool is_static)
    : m_text(text), m_font_name(font_name), m_color(color), m_font_size(font_size), m_is_static(is_static)
{
}

void Text::initialize(AssetManager& asset_manager)
{
  m_font = asset_manager.get<Font>(m_font_name);
  m_has_initialized = true;
  update();
}

void Text::update()
{
  characters.clear();
  assert(m_font != nullptr);
  assert(m_has_initialized);
  const auto scale = m_font_size / static_cast<float>(m_font->get_size());
  float char_pos_x = 0.f;

  for (wchar_t c : m_text)
  {
    const auto& ch = m_font->get_char_data(c);
    const float x = char_pos_x + ch.bl * scale;
    const float y = (m_font->get_max_character_top() - ch.bt) * scale;
    const float w = ch.bw * scale;
    const float h = ch.bh * scale;

    Character character;

    // If the character has a graphical representation
    if (w > 0.f && h > 0.f)
    {
      character.code = c;
      character.sprite = std::make_shared<Sprite>(m_font_name);
      character.sprite->texture = m_font->get_atlas();
      character.sprite->set_custom_uv(ch.tx, ch.bh, ch.bw, ch.bh);

      if (m_font_size != m_font->get_size())
      {
        character.sprite->transform = std::make_shared<Transform>();
        character.sprite->transform->scale.x = scale;
        character.sprite->transform->scale.y = scale;
      }

      character.x = x;
      character.y = y;
      character.w = w;
      character.h = h;
    }
    // Else, the character is an space
    else
    {
      character.code = c;
      character.x = x;
      character.y = y;
    }
    characters.push_back(character);

    char_pos_x += (ch.ax >> 6) * scale;
  }

  m_width = char_pos_x;
  m_height = m_font->get_max_character_top() * scale;
}
}  // namespace dl
