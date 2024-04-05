#pragma once

#include <webgpu/wgpu.h>

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "core/maths/vector.hpp"
#include "graphics/renderer/texture.hpp"

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

class Font
{
 public:
  bool has_loaded = false;
  std::unique_ptr<Texture> texture = nullptr;

  Font(const std::string& path, std::size_t size = 16);
  void load(WGPUDevice device);
  [[nodiscard]] const CharacterData& get_char_data(char32_t c) const
  {
    return ((c >= CHAR_BOTTOM_LIMIT && c < CHAR_TOP_LIMIT) ? m_chars.at(c) : m_empty_char_data);
  };
  [[nodiscard]] inline size_t get_size() const
  {
    return m_size;
  };
  [[nodiscard]] inline int get_max_character_top() const
  {
    return m_max_character_top;
  };

 private:
  std::string m_path{};
  std::size_t m_size;
  std::map<char32_t, CharacterData> m_chars;
  Vector2i m_atlas_size{};
  int m_max_character_top = 0;

  // Limit codes for obtaining characters in the ttf font
  static constexpr int CHAR_BOTTOM_LIMIT = 32;
  static constexpr int CHAR_TOP_LIMIT = 253;

  // Empty data used in case the input is not within the range of loaded chars
  static constexpr CharacterData m_empty_char_data = {0, 0, 0, 0, 0, 0, 0.f};

  // Unicode char ranges to load data from the font file
  static const std::vector<std::pair<char32_t, char32_t>> m_char_ranges;
};
}  // namespace dl
