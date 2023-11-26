#pragma once

#include <memory>
#include <unordered_map>

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
  inline std::pair<bool, bool> get_mouse_state() { return m_mouse_state; };
  inline std::pair<int, int> get_mouse_position() { return m_mouse_position; };
  bool window_size_changed() const { return m_window_size_changed; }
  void set_window_size_changed(bool value) { m_window_size_changed = value; }
  bool should_quit();
  void quit();

 private:
  static const std::unordered_map<std::string, int> m_key_map;
  bool m_any_key_down = false;
  std::unordered_map<int, bool> m_key_down;
  std::unordered_map<int, bool> m_key_up;
  bool m_should_quit = false;
  std::pair<bool, bool> m_mouse_state{false, false};
  std::pair<int, int> m_mouse_position{0, 0};
  bool m_window_size_changed = false;
};
}  // namespace dl
