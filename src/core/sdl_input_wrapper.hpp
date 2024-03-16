#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "./maths/vector.hpp"
#include "core/utf8.hpp"

namespace dl
{
class SDLInputWrapper
{
 public:
  SDLInputWrapper();

  // Removing copy-constructor and assignment operator
  SDLInputWrapper(SDLInputWrapper const&){};
  void operator=(SDLInputWrapper const&){};

  void update();
  void quit();

  bool is_any_key_down() const;
  bool is_key_down(const uint32_t key);
  bool is_key_up(const uint32_t key);
  bool is_dragging() const { return m_is_dragging; }
  bool has_dragged() const { return m_has_dragged; }
  inline std::pair<bool, bool> get_mouse_state_down() const { return m_mouse_state_down; }
  inline std::pair<bool, bool> get_mouse_state_up() const { return m_mouse_state_up; }
  const Vector4i& get_drag_bounds() const { return m_drag_bounds; }
  const Vector2i& get_mouse_position() const { return m_mouse_position; }
  bool window_size_changed() const { return m_window_size_changed; }
  const Vector2i& get_scroll() const { return m_scroll; }
  bool should_quit() const;
  void text_input_start();
  void text_input_stop();
  void reset_drag() { m_has_dragged = false; }
  const std::string& get_text_input() const;

  void set_text_input(const std::string& text);

 private:
  bool m_any_key_down = false;
  std::unordered_map<int, bool> m_key_down;
  std::unordered_map<int, bool> m_key_up;
  bool m_should_quit = false;
  std::pair<bool, bool> m_mouse_state_down{false, false};
  std::pair<bool, bool> m_mouse_state_up{false, false};
  Vector2i m_mouse_position{0, 0};
  Vector2i m_scroll{0, 0};
  Vector4i m_drag_bounds{0, 0, 0, 0};
  bool m_window_size_changed = false;
  bool m_capture_text_input = false;
  bool m_is_dragging = false;
  bool m_has_dragged = false;
  std::string m_text_input{};
  UTF8Iterator m_cursor{m_text_input.end()};
};
}  // namespace dl
