#pragma once
#include <ft2build.h>

#include <map>
#include <utility>
#include <vector>
#include FT_FREETYPE_H
#include <glad/glad.h>

#include "./texture.hpp"
#include "core/asset.hpp"

namespace dl
{
struct CharacterData
{
  long int ax;  // advance.x
  long int ay;  // advance.y
  unsigned bw;  // bitmap.width;
  unsigned bh;  // bitmap.rows;
  int bl;  // bitmap_left;
  int bt;  // bitmap_top;
  float tx;  // x offset of glyph in texture coordinates
};

class Font : public Asset
{
 public:
  bool has_loaded = false;

  Font(const std::string& path, std::size_t size = 16);
  void load();
  const CharacterData& get_char_data(char32_t c) const
  {
    return ((c >= CHAR_BOTTOM_LIMIT && c < CHAR_TOP_LIMIT) ? m_chars.at(c) : m_empty_char_data);
  };
  inline std::shared_ptr<Texture> get_atlas() const { return m_texture_atlas; };
  inline size_t get_size() const { return m_size; };
  inline int get_max_character_top() const { return m_max_character_top; };

 private:
  const char* m_path;
  std::size_t m_size;
  FT_Library m_ft;
  FT_Face m_face;
  std::map<char32_t, CharacterData> m_chars;
  std::shared_ptr<Texture> m_texture_atlas;
  unsigned int m_atlas_width, m_atlas_height;
  int m_max_character_top = 0;

  // Limit codes for obtaining characters in the ttf font
  static const int CHAR_BOTTOM_LIMIT = 32;
  static const int CHAR_TOP_LIMIT = 253;

  // Empty data used in case the input is not within the range of loaded chars
  CharacterData m_empty_char_data = {0, 0, 0, 0, 0, 0, 0.f};

  // Unicode char ranges to load data from the font file
  std::vector<std::pair<char32_t, char32_t>> m_char_ranges = {
      std::make_pair(0x20, 0x7E),  // Basic Latin
      std::make_pair(0xA1, 0x1BF),  // Latin Supplement, Extended-A and part of Extended-B
      std::make_pair(0x1C4, 0x1CC),  // Serbian Cyrillic
      std::make_pair(0x200, 0x21B),  // Additions for Slovenian, Croatian and Romanian
  };
};
}  // namespace dl
