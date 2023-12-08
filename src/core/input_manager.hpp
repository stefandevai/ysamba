#pragma once

#include <memory>
#include <unordered_map>

#include "./json.hpp"
#include "./lua_api.hpp"
#include "./maths/vector.hpp"
#include "./sdl_input_wrapper.hpp"

namespace dl::input_context
{
constexpr std::string_view home_menu = "home_menu";
constexpr std::string_view world_creation = "world_creation";
constexpr std::string_view gameplay = "gameplay";
constexpr std::string_view action_menu = "action_menu";
}  // namespace dl::input_context

namespace dl
{
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
  bool poll_action(const uint32_t action);
  bool is_key_down(const uint32_t key);
  bool is_any_key_down();
  bool is_key_up(const uint32_t key);
  bool is_clicking(const MouseButton button);
  bool has_clicked(const MouseButton button);
  bool is_scrolling_y() const { return m_sdl_input_wrapper.get_scroll().y != 0; }
  bool is_scrolling_x() const { return m_sdl_input_wrapper.get_scroll().x != 0; }
  const Vector2i& get_scroll() const { return m_sdl_input_wrapper.get_scroll(); }
  const Vector2i& get_mouse_position();
  const std::shared_ptr<InputContext> get_current_context();
  bool window_size_changed() const;
  void set_window_size_changed(const bool value);
  bool should_quit();
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
