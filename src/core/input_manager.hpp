#pragma once

#include <memory>
#include <unordered_map>

#include "./json.hpp"
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

  // Removing copy-constructor and assignment operator
  InputManager(InputManager const&){};
  void operator=(InputManager const&){};

  static InputManager& get_instance();
  void update();
  void push_context(const uint32_t context_key);
  void pop_context();

  [[nodiscard]] bool is_context(const uint32_t key);
  [[nodiscard]] bool poll_action(const uint32_t action);
  [[nodiscard]] bool is_key_down(const uint32_t key);
  [[nodiscard]] bool is_any_key_down();
  [[nodiscard]] bool is_key_up(const uint32_t key);
  [[nodiscard]] bool is_clicking(const MouseButton button);
  [[nodiscard]] bool has_clicked(const MouseButton button);
  [[nodiscard]] bool has_clicked_bounds(const MouseButton button, const Vector2i& position, const Vector2i& size);
  [[nodiscard]] bool is_scrolling_y() const { return m_sdl_input_wrapper.get_scroll().y != 0; }
  [[nodiscard]] bool is_scrolling_x() const { return m_sdl_input_wrapper.get_scroll().x != 0; }
  [[nodiscard]] const Vector2i& get_scroll() const { return m_sdl_input_wrapper.get_scroll(); }
  [[nodiscard]] const Vector2i& get_mouse_position() const;
  [[nodiscard]] const Vector2i get_mouse_tile_position(const Camera& camera) const;
  [[nodiscard]] const Vector3i get_tile_at_mouse_position(const World& world, const Camera& camera) const;
  [[nodiscard]] const std::shared_ptr<InputContext> get_current_context();
  [[nodiscard]] bool window_size_changed() const;
  void text_input_start();
  void text_input_stop();
  const std::string& get_text_input() const;

  void set_text_input(const std::string& text);
  void set_window_size_changed(const bool value);
  [[nodiscard]] bool should_quit();
  void quit();

 private:
  JSON m_json{"./data/input.json"};
  static std::unique_ptr<InputManager> m_instance;
  static SDLInputWrapper m_sdl_input_wrapper;
  static std::unordered_map<uint32_t, std::shared_ptr<InputContext>> m_available_contexts;
  static std::vector<std::shared_ptr<InputContext>> m_context_stack;

  void m_parse_input();
};
}  // namespace dl
