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
    : value(text), typeface(typeface), font_size(font_size), m_is_static(is_static)
{
}

void Text::initialize(AssetManager& asset_manager)
{
  assert(typeface != 0 && "Failed to initialize text, typeface is 0");

  if (font == nullptr)
  {
    font = asset_manager.get<Font>(typeface);
  }

  assert(font != nullptr && "Failed to initialize text, font is nullptr");
  update();
  has_initialized = true;
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
  assert(font != nullptr);
  const auto scale = font_size / static_cast<float>(font->get_size());
  float char_pos_x = 0.f;
  float char_pos_y = 0.f;
  const auto max_character_top = font->get_max_character_top();

  auto last_word_index = characters.begin();
  Color character_color = color;

  assert(wrap_width != 0);

  for (UTF8Iterator it = value.begin(); it != value.end(); ++it)
  {
    auto ch = font->get_char_data(*it);

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

    // If character is a space or another invisible character, set sprite as null and correct dimensions
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
      ch = font->get_char_data(*it);
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
    character.slice = std::make_unique<TextureSlice>();
    character.slice->texture = font->texture.get();
    character.slice->color = character_color;
    character.slice->set_uv_with_size(ch.bh, ch.tx, ch.bw, ch.bh);

    assert(typeface != 0);
    assert(character.slice->texture != nullptr);

    // if (font_size != font->get_size())
    // {
    //   character.slice->transform = std::make_unique<Transform>();
    //   character.slice->transform->scale.x = scale;
    //   character.slice->transform->scale.y = scale;
    // }

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
  assert(font != nullptr);

  characters.clear();

  const auto scale = font_size / static_cast<float>(font->get_size());
  float char_pos_x = 0.f;

  if (value.empty())
  {
    return;
  }

  for (UTF8Iterator it = value.begin(); it != value.end(); ++it)
  {
    const auto& ch = font->get_char_data(*it);
    const float x = char_pos_x + ch.bl * scale;
    const float y = (font->get_max_character_top() - ch.bt) * scale;
    const float w = ch.bw * scale;
    const float h = ch.bh * scale;

    Character character;

    character.code = *it;

    // If character is a space or another invisible character, set sprite as null and correct dimensions
    if (w == 0.0 || h == 0.0)
    {
      character.slice = nullptr;
      character.w = (ch.ax >> 6) * scale;
      character.h = font_size;
    }
    else
    {
      character.slice = std::make_unique<TextureSlice>();
      character.slice->texture = font->texture.get();
      character.slice->color = color;
      character.slice->set_uv_with_size(ch.bh, ch.tx, ch.bw, ch.bh);
      character.w = w;
      character.h = h;

      assert(typeface != 0);
      assert(character.slice->texture != nullptr);
    }

    // if (font_size != font->get_size())
    // {
    //   character.slice->transform = std::make_unique<Transform>();
    //   character.slice->transform->scale.x = scale;
    //   character.slice->transform->scale.y = scale;
    // }

    character.x = x;
    character.y = y;

    characters.push_back(std::move(character));

    char_pos_x += (ch.ax >> 6) * scale;
  }

  m_size.x = char_pos_x;
  m_size.y = font->get_max_character_top() * scale;
}

Vector2i Text::get_position_at(const int index) const
{
  Vector2i position{-1, -1};

  if (index < 1 || static_cast<uint32_t>(index) > characters.size())
  {
    return position;
  }

  const int char_index = index - 1;
  const int char_y = characters[char_index].y;

  position.y = (char_y / font_size) * font_size;
  position.x = characters[char_index].x + characters[char_index].w;

  return position;
}

void Text::set_typeface(const uint32_t typeface)
{
  this->typeface = typeface;
  has_initialized = false;
}

void Text::set_text(const std::string_view text)
{
  value = text;
  wrap_width = 0;
  has_initialized = false;
}

void Text::set_text_wrapped(const std::string_view text, const int wrap_width)
{
  value = text;
  this->wrap_width = wrap_width;
  has_initialized = false;
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
