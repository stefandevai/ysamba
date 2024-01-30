#include "./font.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <glm/glm.hpp>  // IWYU pragma: export
#include <string>

namespace dl
{
Font::Font(const std::string& path, std::size_t size) : m_path(path.c_str()), m_size(size) {}

void Font::load()
{
  if (FT_Init_FreeType(&m_ft))
  {
    spdlog::critical("Could not init FreeType Library");
  }
  if (FT_New_Face(m_ft, m_path, 0, &m_face))
  {
    spdlog::critical("Failed to load font {}", m_path);
  }
  if (FT_Select_Charmap(m_face, FT_ENCODING_UNICODE))
  {
    spdlog::critical("Failed to load unicode charmap");
  }
  FT_Set_Pixel_Sizes(m_face, 0, m_size);

  uint32_t atlas_width = 0;
  uint32_t atlas_height = 0;

  for (const auto& char_range : m_char_ranges)
  {
    for (char32_t c = char_range.first; c < char_range.second; c++)
    {
      if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
      {
        spdlog::critical("Failed to load Glyph");
        continue;
      }
      atlas_width += m_face->glyph->bitmap.width;
      atlas_height = std::max(atlas_height, m_face->glyph->bitmap.rows);
    }
  }

  m_atlas_width = atlas_width;
  m_atlas_height = atlas_height;

  m_texture_atlas = std::make_shared<Texture>(atlas_width, atlas_height);

  int x_offset = 0;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  m_texture_atlas->bind();

  for (const auto& char_range : m_char_ranges)
  {
    for (char32_t c = char_range.first; c < char_range.second; c++)
    {
      if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
      {
        spdlog::critical("Failed to load Glyph");
        continue;
      }
      glTexSubImage2D(GL_TEXTURE_2D,
                      0,
                      x_offset,
                      0,
                      m_face->glyph->bitmap.width,
                      m_face->glyph->bitmap.rows,
                      GL_RED,
                      GL_UNSIGNED_BYTE,
                      m_face->glyph->bitmap.buffer);

      m_max_character_top = std::max(m_max_character_top, m_face->glyph->bitmap_top);

      CharacterData ch_data = {m_face->glyph->advance.x,
                               m_face->glyph->advance.y,
                               m_face->glyph->bitmap.width,
                               m_face->glyph->bitmap.rows,
                               m_face->glyph->bitmap_left,
                               m_face->glyph->bitmap_top,
                               (float)x_offset / m_atlas_width};
      m_chars.insert(std::pair<char32_t, CharacterData>(c, ch_data));
      x_offset += m_face->glyph->bitmap.width;
    }
  }

  // Use the red value in the GBA channels to facilitate the work in the shader
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

  m_texture_atlas->unbind();

  FT_Done_Face(m_face);
  FT_Done_FreeType(m_ft);
}
}  // namespace dl
