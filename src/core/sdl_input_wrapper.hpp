#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "core/maths/vector.hpp"
#include "core/mouse_cursor.hpp"
#include "core/timer.hpp"
#include "core/utf8.hpp"

struct SDL_Cursor;

namespace dl
{
class SDLInputWrapper
{
 public:
  SDLInputWrapper() = default;
  ~SDLInputWrapper();

  // Remove copy/move constructors and assignment operators
  SDLInputWrapper(SDLInputWrapper const&) = delete;
  SDLInputWrapper(SDLInputWrapper const&&) = delete;
  void operator=(SDLInputWrapper const&) = delete;
  void operator=(SDLInputWrapper const&&) = delete;

  void update();
  void quit();

  [[nodiscard]] bool is_any_key_down() const;
  [[nodiscard]] bool is_key_down(uint32_t key);
  [[nodiscard]] bool is_key_up(uint32_t key);
  [[nodiscard]] bool is_dragging() const { return m_is_dragging; }
  [[nodiscard]] bool has_dragged() const { return m_has_dragged; }
  [[nodiscard]] inline std::pair<bool, bool> get_mouse_state_down() const { return m_mouse_state_down; }
  [[nodiscard]] inline std::pair<bool, bool> get_mouse_state_up() const { return m_mouse_state_up; }
  [[nodiscard]] const Vector4i& get_drag_bounds() const { return m_drag_bounds; }
  [[nodiscard]] const Vector2i& get_mouse_position() const { return m_mouse_position; }
  [[nodiscard]] bool window_size_changed() const { return m_window_size_changed; }
  [[nodiscard]] const Vector2i& get_scroll() const { return m_scroll; }
  [[nodiscard]] bool should_quit() const;
  [[nodiscard]] const std::string& get_text_input() const;
  [[nodiscard]] int get_text_input_cursor_index() const;
  void text_input_start();
  void text_input_stop();
  void reset_drag() { m_has_dragged = false; }
  void set_mouse_cursor(MouseCursor cursor);

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
  UTF8Iterator m_cursor{m_text_input.begin()};
  Timer m_mouse_down_timer{};
  MouseCursor m_mouse_cursor = MouseCursor::Arrow;
  MouseCursor m_last_mouse_cursor = MouseCursor::Arrow;
  SDL_Cursor* m_sdl_cursor = nullptr;
};
}  // namespace dl
