#pragma once

#include <string>
#include <vector>

namespace dl
{
class Sprite;
class AssetManager;

struct Character
{
  wchar_t code;
  int x;
  int y;
  int w;
  int h;
  std::shared_ptr<Sprite> sprite;
};

class Text
{
 public:
  std::vector<Character> characters{};

  Text() {}
  Text(const std::wstring text,
       const std::string font_name,
       const unsigned int font_size,
       const std::string& color = "#ffffffff",
       const bool is_static = true);

  void initialize(AssetManager& asset_manager);

  inline std::wstring get_text() const { return m_text; };
  inline std::string get_color() const { return m_color; };
  inline std::string get_font_name() const { return m_font_name; };
  inline bool get_is_static() const { return m_is_static; };
  inline size_t get_font_size() const { return m_font_size; };
  inline bool get_has_initialized() const { return m_has_initialized; };

  inline void set_text(const std::wstring& text) { m_text = text; };
  inline void set_font_name(const std::string& font_name) { m_font_name = font_name; };
  inline void set_font_size(const size_t font_size) { m_font_size = font_size; };
  inline void set_color(const std::string& color) { m_color = color; };
  inline void set_is_static(const bool is_static) { m_is_static = is_static; };

 private:
  std::wstring m_text;
  std::string m_font_name, m_color;
  size_t m_font_size = 32;
  bool m_is_static = true;
  bool m_has_initialized = false;
  int m_width = 0;
  int m_height = 0;
};
}  // namespace dl
