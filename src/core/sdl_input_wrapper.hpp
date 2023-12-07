#pragma once

#include <memory>
#include <unordered_map>

#include "./maths/vector.hpp"

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
  bool is_any_key_down();
  bool is_key_down(const std::string& key);
  bool is_key_up(const std::string& key);
  inline std::pair<bool, bool> get_mouse_state_down() { return m_mouse_state_down; };
  inline std::pair<bool, bool> get_mouse_state_up() { return m_mouse_state_up; };
  const Vector2i& get_mouse_position() { return m_mouse_position; };
  bool window_size_changed() const { return m_window_size_changed; }
  const Vector2i& get_scroll() const { return m_scroll; }
  void set_window_size_changed(bool value) { m_window_size_changed = value; }
  bool should_quit();
  void quit();

 private:
  bool m_any_key_down = false;
  std::unordered_map<int, bool> m_key_down;
  std::unordered_map<int, bool> m_key_up;
  bool m_should_quit = false;
  std::pair<bool, bool> m_mouse_state_down{false, false};
  std::pair<bool, bool> m_mouse_state_up{false, false};
  Vector2i m_mouse_position{0, 0};
  Vector2i m_scroll{0, 0};
  bool m_window_size_changed = false;
};
}  // namespace dl
