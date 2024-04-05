#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "./maths/vector.hpp"
#include "./sdl_input_wrapper.hpp"

namespace dl
{
class Camera;
class World;

struct InputContext
{
  uint32_t key;
  std::unordered_map<uint32_t, std::vector<uint32_t>> actions;

  InputContext(const uint32_t key) : key(key) {}
};

class InputManager
{
 public:
  enum class MouseButton
  {
    Left,
    Right,
  };

  InputManager();
  ~InputManager() = default;

  // Removing copy/move constructors and assignment operators
  InputManager(InputManager const&) = delete;
  InputManager(InputManager const&&) = delete;
  void operator=(InputManager const&) = delete;
  void operator=(InputManager const&&) = delete;

  static InputManager& get_instance();
  void update();
  void push_context(uint32_t context_key);
  void pop_context();

  [[nodiscard]] bool is_context(uint32_t key) const;
  [[nodiscard]] bool poll_action(uint32_t action);
  [[nodiscard]] bool is_key_down(uint32_t key);
  [[nodiscard]] bool is_any_key_down() const;
  [[nodiscard]] bool is_key_up(uint32_t key) const;
  [[nodiscard]] bool is_clicking(MouseButton button) const;
  [[nodiscard]] bool has_clicked(MouseButton button) const;
  [[nodiscard]] bool has_clicked_bounds(MouseButton button, const Vector2i& position, const Vector2i& size) const;
  [[nodiscard]] static bool is_scrolling_y() { return m_sdl_input_wrapper.get_scroll().y != 0; }
  [[nodiscard]] static bool is_scrolling_x() { return m_sdl_input_wrapper.get_scroll().x != 0; }
  [[nodiscard]] bool is_dragging() const;
  [[nodiscard]] bool has_dragged() const;
  [[nodiscard]] static const Vector2i& get_scroll() { return m_sdl_input_wrapper.get_scroll(); }
  [[nodiscard]] const Vector2i& get_mouse_position() const;
  [[nodiscard]] const Vector4i& get_drag_bounds() const;
  [[nodiscard]] const std::shared_ptr<InputContext> get_current_context() const;
  [[nodiscard]] bool window_size_changed() const;
  void text_input_start();
  void text_input_stop();
  [[nodiscard]] const std::string& get_text_input() const;
  [[nodiscard]] int get_text_input_cursor_index() const;
  void reset_drag();

  [[nodiscard]] bool should_quit() const;
  void quit();
  void set_text_input(const std::string& text);

 private:
  static std::unique_ptr<InputManager> m_instance;
  static SDLInputWrapper m_sdl_input_wrapper;
  static std::unordered_map<uint32_t, std::shared_ptr<InputContext>> m_available_contexts;
  static std::vector<std::shared_ptr<InputContext>> m_context_stack;

  void m_parse_input();
};
}  // namespace dl
