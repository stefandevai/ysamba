#include "./text.hpp"

#include <spdlog/spdlog.h>

#include "./font.hpp"
#include "core/asset_manager.hpp"
#include "core/utf8.hpp"

namespace dl
{
Text::Text(const std::string_view text,
           const std::string_view typeface,
           const unsigned int font_size,
           const bool is_static)
    : value(text), typeface(typeface), m_font_size(font_size), m_is_static(is_static)
{
  initialize();
}

void Text::initialize()
{
  m_font = AssetManager::get<Font>(typeface);
  m_has_initialized = true;
  update();
}

void Text::update()
{
  if (!m_has_initialized)
  {
    initialize();
  }

  characters.clear();
  assert(m_font != nullptr);
  assert(m_has_initialized);
  const auto scale = m_font_size / static_cast<float>(m_font->get_size());
  float char_pos_x = 0.f;

  for (UTF8Iterator it = value.begin(); it != value.end(); ++it)
  {
    const auto& ch = m_font->get_char_data(*it);
    const float x = char_pos_x + ch.bl * scale;
    const float y = (m_font->get_max_character_top() - ch.bt) * scale;
    const float w = ch.bw * scale;
    const float h = ch.bh * scale;

    // Skip if character is a space
    if (*it == 0x20)
    {
      char_pos_x += (ch.ax >> 6) * scale;
      continue;
    }

    Character character;

    character.code = *it;
    character.sprite = std::make_unique<Sprite>(typeface);
    character.sprite->texture = m_font->get_atlas();
    character.sprite->set_custom_uv(ch.bh, ch.tx, ch.bw, ch.bh);
    character.sprite->color = color;

    if (m_font_size != m_font->get_size())
    {
      character.sprite->transform = std::make_unique<Transform>();
      character.sprite->transform->scale.x = scale;
      character.sprite->transform->scale.y = scale;
    }

    character.x = x;
    character.y = y;
    character.w = w;
    character.h = h;

    characters.push_back(std::move(character));

    char_pos_x += (ch.ax >> 6) * scale;
  }

  m_size.x = char_pos_x;
  m_size.y = m_font->get_max_character_top() * scale;
}

void Text::set_typeface(const std::string_view typeface)
{
  this->typeface = typeface;
  initialize();
}

void Text::set_text(const std::string_view text)
{
  value = text;
  update();
}

void Text::set_text_wrapped(const std::string_view text, const int wrap_width)
{
  value = text;

  if (!m_has_initialized)
  {
    initialize();
  }

  characters.clear();
  assert(m_font != nullptr);
  assert(m_has_initialized);
  const auto scale = m_font_size / static_cast<float>(m_font->get_size());
  float char_pos_x = 0.f;
  float char_pos_y = 0.f;
  const auto max_character_top = m_font->get_max_character_top();

  auto last_word_index = characters.begin();

  for (UTF8Iterator it = value.begin(); it != value.end(); ++it)
  {
    auto ch = m_font->get_char_data(*it);

    // Break to new line if width is bigger than wrap_width
    if (char_pos_x > wrap_width)
    {
      auto last_element = characters.end() - 1;

      while (last_element != last_word_index && last_element != characters.begin())
      {
        characters.pop_back();
        last_element = characters.end() - 1;
        --it;
      }

      char_pos_y += max_character_top * scale * line_height;
      char_pos_x = 0.f;

      // Skip if character is a space without aggregating to the total width
      if (*it == 0x20)
      {
        continue;
      }

      ch = m_font->get_char_data(*it);
    }

    // Skip if character is a space
    if (*it == 0x20)
    {
      last_word_index = characters.end() - 1;
      char_pos_x += (ch.ax >> 6) * scale;
      continue;
    }
    else if (*it == 0x0a)
    {
      last_word_index = characters.end() - 1;
      char_pos_x = 0;
      char_pos_y += max_character_top * scale * line_height;
      continue;
    }

    const float x = char_pos_x + ch.bl * scale;
    const float y = char_pos_y + (max_character_top - ch.bt) * scale;
    const float w = ch.bw * scale;
    const float h = ch.bh * scale;

    Character character;

    character.code = *it;
    character.sprite = std::make_unique<Sprite>(typeface);
    character.sprite->texture = m_font->get_atlas();
    character.sprite->set_custom_uv(ch.bh, ch.tx, ch.bw, ch.bh);
    character.sprite->color = color;

    if (m_font_size != m_font->get_size())
    {
      character.sprite->transform = std::make_unique<Transform>();
      character.sprite->transform->scale.x = scale;
      character.sprite->transform->scale.y = scale;
    }

    character.x = x;
    character.y = y;
    character.w = w;
    character.h = h;

    characters.push_back(std::move(character));

    char_pos_x += (ch.ax >> 6) * scale;
  }

  m_size.x = char_pos_x;
  m_size.y = char_pos_y;
}
}  // namespace dl
