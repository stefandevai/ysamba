#include "./text.hpp"

#include <spdlog/spdlog.h>

#include "./font.hpp"
#include "core/asset_manager.hpp"
#include "core/utf8.hpp"

namespace dl
{
constexpr uint32_t unicode_space = 0x20;
constexpr uint32_t unicode_new_line = 0x0A;
constexpr uint32_t unicode_backslash = 0x5C;
constexpr uint32_t unicode_bracket_left = 0x5B;
constexpr uint32_t unicode_bracket_right = 0x5D;
constexpr uint32_t unicode_slash = 0x2F;
constexpr uint32_t unicode_hash = 0x23;

Text::Text(const std::string_view text, const uint32_t typeface, const unsigned int font_size, const bool is_static)
    : value(text), typeface(typeface), m_font_size(font_size), m_is_static(is_static)
{
}

void Text::initialize(AssetManager& asset_manager)
{
  assert(typeface != 0 && "Failed to initialize text, typeface is 0");

  if (m_font == nullptr)
  {
    m_font = asset_manager.get<Font>(typeface);
  }

  assert(m_font != nullptr && "Failed to initialize text, font is nullptr");
  update();
  m_has_initialized = true;
}

void Text::update()
{
  if (wrap_width > 0)
  {
    update_wrapped();
  }
  else
  {
    update_non_wrapped();
  }
}

void Text::update_wrapped()
{
  characters.clear();
  assert(m_font != nullptr);
  const auto scale = m_font_size / static_cast<float>(m_font->get_size());
  float char_pos_x = 0.f;
  float char_pos_y = 0.f;
  const auto max_character_top = m_font->get_max_character_top();

  auto last_word_index = characters.begin();
  Color character_color = color;

  for (UTF8Iterator it = value.begin(); it != value.end(); ++it)
  {
    auto ch = m_font->get_char_data(*it);

    // Break to new line if width is bigger than wrap_width
    if (char_pos_x > wrap_width)
    {
      // Pop characters, get iterator back to the last word
      while (true)
      {
        auto last_element = characters.end() - 1;
        --it;

        if (last_element == last_word_index || last_element == characters.begin())
        {
          break;
        }

        characters.pop_back();
      }

      // Set y position to the next line and reset x position
      char_pos_y += max_character_top * scale * line_height;
      char_pos_x = 0.f;

      continue;
    }

    // Skip if character is a space
    if (*it == unicode_space)
    {
      last_word_index = characters.end() - 1;
      char_pos_x += (ch.ax >> 6) * scale;
      continue;
    }
    // Character is a newline break "\n"
    else if (*it == unicode_new_line)
    {
      last_word_index = characters.end() - 1;
      char_pos_x = 0;
      char_pos_y += max_character_top * scale * line_height;
      continue;
    }
    // Escape character "\"
    else if (*it == unicode_backslash)
    {
      ++it;
      ch = m_font->get_char_data(*it);
    }
    // Character is part of a command "["
    else if (*it == unicode_bracket_left)
    {
      m_process_command(it, character_color);
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
    character.sprite->color = character_color;

    assert(typeface != 0);
    assert(character.sprite->texture != nullptr);

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
  m_size.y = char_pos_y + max_character_top * scale;
}

void Text::update_non_wrapped()
{
  assert(m_font != nullptr);

  characters.clear();

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
    if (w == 0.0 || h == 0.0)
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

    assert(typeface != 0);
    assert(character.sprite->texture != nullptr);

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

void Text::set_typeface(const uint32_t typeface)
{
  this->typeface = typeface;
  m_has_initialized = false;
}

void Text::set_text(const std::string_view text)
{
  value = text;
  wrap_width = 0;
  m_has_initialized = false;
}

void Text::set_text_wrapped(const std::string_view text, const int wrap_width)
{
  value = text;
  this->wrap_width = wrap_width;
  m_has_initialized = false;
}

void Text::m_process_command(UTF8Iterator& it, Color& character_color)
{
  ++it;

  const auto command = *it;

  switch (command)
  {
  // Character to finish last command
  case unicode_slash:
  {
    ++it;

    if (*it == unicode_hash)
    {
      character_color = color;
    }
    break;
  }

  // Color command
  case unicode_hash:
  {
    const auto hex_color = std::string(it.string_iterator + 1, it.string_iterator + 7);
    character_color = Color{std::move(hex_color)};
    break;
  }

  default:
    break;
  }

  // Skip command characters
  while (*it != unicode_bracket_right && it != value.end())
  {
    ++it;
  }
}

}  // namespace dl
