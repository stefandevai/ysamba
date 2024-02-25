#include "./font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <spdlog/spdlog.h>

#include <algorithm>
#include <glm/glm.hpp>
#include <string>

namespace dl
{
Font::Font(const std::string& path, std::size_t size) : m_path(path), m_size(size) {}

void Font::load(const WGPUDevice device)
{
  FT_Library ft;
  FT_Face face;

  if (FT_Init_FreeType(&ft))
  {
    spdlog::critical("Could not init FreeType Library");
    return;
  }
  if (FT_New_Face(ft, m_path.c_str(), 0, &face))
  {
    spdlog::critical("Failed to load font {}", m_path);
    return;
  }
  if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
  {
    spdlog::critical("Failed to load unicode charmap");
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return;
  }
  FT_Set_Pixel_Sizes(face, 0, m_size);

  uint32_t atlas_width = 0;
  uint32_t atlas_height = 0;

  for (const auto& char_range : m_char_ranges)
  {
    for (char32_t c = char_range.first; c < char_range.second; c++)
    {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
        spdlog::critical("Failed to load Glyph");
        continue;
      }
      atlas_width += face->glyph->bitmap.width;
      atlas_height = std::max(atlas_height, face->glyph->bitmap.rows);
    }
  }

  m_atlas_width = atlas_width;
  m_atlas_height = atlas_height;

  m_texture_atlas = std::make_unique<Texture>();

  int x_offset = 0;

  std::vector<unsigned char> data(atlas_width * atlas_height, 0);

  for (const auto& char_range : m_char_ranges)
  {
    for (char32_t c = char_range.first; c < char_range.second; c++)
    {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
        spdlog::critical("Failed to load Glyph");
        continue;
      }

      const auto& glyph = *face->glyph;

      for (uint32_t j = 0; j < glyph.bitmap.rows; ++j)
      {
        for (uint32_t i = 0; i < glyph.bitmap.width; ++i)
        {
          data[x_offset + i + j * atlas_width] = glyph.bitmap.buffer[i + j * glyph.bitmap.width];
        }
      }

      m_max_character_top = std::max(m_max_character_top, face->glyph->bitmap_top);

      CharacterData ch_data = {glyph.advance.x,
                               glyph.advance.y,
                               glyph.bitmap.width,
                               glyph.bitmap.rows,
                               glyph.bitmap_left,
                               glyph.bitmap_top,
                               (float)x_offset / m_atlas_width};
      m_chars.insert(std::pair<char32_t, CharacterData>(c, ch_data));
      x_offset += glyph.bitmap.width;
    }
  }

  m_texture_atlas->load(device, data.data(), m_atlas_width, m_atlas_height, 1);

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  has_loaded = true;
}
}  // namespace dl
