#pragma once

#include <entt/core/hashed_string.hpp>
#include <string>
#include <vector>

#include "core/maths/vector.hpp"
#include "graphics/sprite.hpp"

namespace dl
{
using namespace entt::literals;

class Font;
class Sprite;
class UTF8Iterator;

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
  std::string value = "";
  uint32_t typeface = "font-1980"_hs;
  std::vector<Character> characters{};
  Color color{0xFFFFFFFF};
  double line_height = 1.2;

  Text(const std::string_view text,
       const uint32_t typeface,
       const unsigned int font_size = 16,
       const bool is_static = true);
  Text() = default;
  Text(Text&&) = default;
  Text(const Text&) = delete;
  Text& operator=(const Text&) = delete;
  Text& operator=(Text&&) = default;

  void initialize();
  void update();

  const std::string& get_text() const { return value; }
  uint32_t get_typeface() const { return typeface; }
  bool get_is_static() const { return m_is_static; }
  size_t get_font_size() const { return m_font_size; }
  bool get_has_initialized() const { return m_has_initialized; }
  Vector2i get_size() const { return m_size; }

  void set_text(const std::string_view text);
  void set_text_wrapped(const std::string_view text, const int wrap_width);
  void set_typeface(const uint32_t typeface);
  void set_font_size(const size_t font_size) { m_font_size = font_size; }
  void set_is_static(const bool is_static) { m_is_static = is_static; }

 private:
  size_t m_font_size = 16;
  bool m_is_static = true;
  bool m_has_initialized = false;
  Vector2i m_size{0, 0};
  const Font* m_font = nullptr;

  void m_process_command(UTF8Iterator& it, Color& character_color);
};
}  // namespace dl
