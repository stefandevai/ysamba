#include "./text.hpp"

#include <spdlog/spdlog.h>
#include "../core/asset_manager.hpp"
#include "./sprite.hpp"
#include "./font.hpp"

namespace dl
{
  Text::Text (const std::wstring text, const std::string font_name, const unsigned int font_size, const std::string& color, const bool is_static)
    : m_text (text), m_font_name (font_name), m_color (color), m_font_size (font_size), m_is_static (is_static)
  {
  }

  void Text::initialize (AssetManager& asset_manager)
  {
    auto font        = asset_manager.get<Font> (m_font_name);
    const auto scale = m_font_size / static_cast<float> (font->get_size());
    float char_pos_x  = 0.f;

    for (wchar_t c : m_text)
    {
      const auto& ch     = font->get_char_data (c);
      const float x = char_pos_x + ch.bl * scale;
      const float y = (font->get_max_character_top() - ch.bt) * scale;
      const float w    = ch.bw * scale;
      const float h    = ch.bh * scale;

      /* auto char_entity = registry.create(); */
      Character character;

      // If the character has a graphical representation
      if (w > 0.f && h > 0.f)
      {
        character.code = c;
        character.sprite = std::make_shared<Sprite>(m_font_name);
        character.sprite->set_texture (font->get_atlas());
        character.sprite->set_custom_uv (ch.tx, ch.bh, ch.bw, ch.bh);

        /* registry.emplace<component::Charcode> (char_entity, c); */
        /* auto& sprite = registry.emplace<component::Sprite> (char_entity, text.get_font_name()); */
        /* sprite.set_texture (font->get_atlas()); */
        /* sprite.set_custom_uv (ch.tx, ch.bh, ch.bw, ch.bh); */


        /* spdlog::warn("HEREEEEEEEEEEEEEEEEEEEEE"); */
        /* spdlog::warn("font size: {}", m_font_size); */
        /* spdlog::warn("font size original: {}", font->get_size()); */
        if (m_font_size != font->get_size())
        {
          character.sprite->transform = std::make_shared<Transform>();
          character.sprite->transform->scale.x = scale;
          character.sprite->transform->scale.y = scale;
        }

        character.x = x;
        character.y = y;
        character.w = w;
        character.h = h;

        /* registry.emplace<component::Position> (char_entity, pos.x + xpos, pos.y + ypos, 30.0f); */
        /* registry.emplace<component::Dimension> (char_entity, w, h); */
        /* registry.emplace<component::Color> (char_entity, text.get_color()); */
      }
      // Else, the character is an space
      else
      {
        character.code = c;
        character.x = x;
        character.y = y;
        /* registry.emplace<component::Charcode> (char_entity, c); */
        /* registry.emplace<component::Position> (char_entity, pos.x + xpos, pos.y + ypos); */
      }
      characters.push_back(character);
      /* text.char_entities.push_back (char_entity); */

      char_pos_x += (ch.ax >> 6) * scale;
      /* char_maxh = std::max (char_maxh, h); */
    }

    /* auto& dim = registry.get_or_emplace<component::Dimension, float, float> (entity, 0.f, 0.f); */
    m_width = char_pos_x;
    m_height = font->get_max_character_top() * scale;
    m_has_initialized = true;
  }
}
