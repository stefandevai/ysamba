#pragma once

#include <string>
#include <vector>

#include "core/maths/vector.hpp"
#include "graphics/sprite.hpp"

namespace dl
{
class Font;

struct Character
{
  wchar_t code;
  int x;
  int y;
  int w;
  int h;
  std::unique_ptr<Sprite> sprite = nullptr;
};

class Text
{
 public:
  std::string value, typeface, color;
  std::vector<Character> characters{};

  Text(const std::string_view text,
       const std::string_view typeface = "font-1980",
       const unsigned int font_size = 32,
       const std::string_view color = "#ffffffff",
       const bool is_static = true);
  Text() = default;
  Text(Text&&) = default;
  Text(const Text&) = delete;
  Text& operator=(const Text&) = delete;
  Text& operator=(Text&&) = default;

  void initialize();
  void update();

  const std::string& get_text() const { return value; }
  const std::string& get_color() const { return color; }
  const std::string& get_typeface() const { return typeface; }
  bool get_is_static() const { return m_is_static; }
  size_t get_font_size() const { return m_font_size; }
  bool get_has_initialized() const { return m_has_initialized; }
  Vector2i get_size() const { return m_size; }

  /* void set_text(const std::string& text) { value = text; } */
  /* void set_typeface(const std::string& typeface) { this->typeface = typeface; } */
  void set_text(const std::string_view text);
  void set_typeface(const std::string_view typeface);
  void set_font_size(const size_t font_size) { m_font_size = font_size; }
  void set_color(const std::string_view color) { this->color = color; }
  void set_is_static(const bool is_static) { m_is_static = is_static; }

 private:
  size_t m_font_size = 32;
  bool m_is_static = true;
  bool m_has_initialized = false;
  Vector2i m_size{0, 0};
  std::shared_ptr<Font> m_font = nullptr;
};
}  // namespace dl
