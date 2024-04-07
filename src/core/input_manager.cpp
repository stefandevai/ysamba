#include "./input_manager.hpp"

#include <SDL.h>
#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <vector>

#include "config.hpp"
#include "core/json.hpp"
#include "graphics/camera.hpp"
#include "world/world.hpp"

namespace dl
{
std::unique_ptr<InputManager> InputManager::m_instance = nullptr;
SDLInputWrapper InputManager::m_sdl_input_wrapper = SDLInputWrapper();
std::unordered_map<uint32_t, std::shared_ptr<InputContext>> InputManager::m_available_contexts = {};
std::vector<std::shared_ptr<InputContext>> InputManager::m_context_stack = {};

InputManager::InputManager()
{
  m_parse_input();
}

InputManager& InputManager::get_instance()
{
  if (m_instance == nullptr)
  {
    m_instance = std::make_unique<InputManager>();
  }

  return *m_instance;
}

void InputManager::update()
{
  m_sdl_input_wrapper.update();
}

void InputManager::push_context(const uint32_t context_key)
{
  if (!m_available_contexts.contains(context_key))
  {
    spdlog::critical("Input context not found: {}", context_key);
    return;
  }

  const auto& context = m_available_contexts.at(context_key);
  m_context_stack.push_back(context);
}

void InputManager::pop_context()
{
  if (m_context_stack.empty())
  {
    spdlog::warn("No context available for poping, quitting game");
    quit();
    return;
  }

  m_context_stack.pop_back();
}

bool InputManager::is_context(const uint32_t key) const
{
  const auto& current_context = m_context_stack.back();

  if (current_context == nullptr || current_context->key != key)
  {
    return false;
  }

  return true;
}

bool InputManager::poll_action(const uint32_t action_key)
{
  if (m_context_stack.empty())
  {
    spdlog::critical("No contexts avaiblable");
    return false;
  }

  const auto current_context = m_context_stack.back();

  if (!current_context->actions.contains(action_key))
  {
    spdlog::warn("Could not find input action in this context: {}", action_key);
    return false;
  }

  for (const auto& action : current_context->actions.at(action_key))
  {
    if (is_key_down(action))
    {
      return true;
    }
  }

  return false;
}

bool InputManager::is_key_down(const uint32_t key)
{
  return m_sdl_input_wrapper.is_key_down(key);
}

bool InputManager::is_any_key_down() const
{
  return m_sdl_input_wrapper.is_any_key_down();
}

bool InputManager::is_key_up(const uint32_t key) const
{
  return m_sdl_input_wrapper.is_key_up(key);
}

bool InputManager::is_clicking(const MouseButton button) const
{
  const auto& mouse_state = m_sdl_input_wrapper.get_mouse_state_down();

  if (button == MouseButton::Left)
  {
    return mouse_state.first;
  }
  else if (button == MouseButton::Right)
  {
    return mouse_state.second;
  }

  return false;
}

bool InputManager::has_clicked(const MouseButton button) const
{
  const auto& mouse_state = m_sdl_input_wrapper.get_mouse_state_up();

  if (button == MouseButton::Left)
  {
    return mouse_state.first;
  }
  else if (button == MouseButton::Right)
  {
    return mouse_state.second;
  }

  return false;
}

bool InputManager::has_clicked_aabb(const MouseButton button, const Vector2i& position, const Vector2i& size) const
{
  if (has_clicked(button))
  {
    const auto& mouse_position = get_mouse_position();

    if (mouse_position.x > position.x && mouse_position.x < position.x + size.x && mouse_position.y > position.y
        && mouse_position.y < position.y + size.y)
    {
      return true;
    }
  }

  return false;
}

bool InputManager::mouse_hover_aabb(const Vector2i& position, const Vector2i& size) const
{
  const auto& mouse_position = get_mouse_position();

  if (mouse_position.x > position.x && mouse_position.x < position.x + size.x && mouse_position.y > position.y
      && mouse_position.y < position.y + size.y)
  {
    return true;
  }

  return false;
}

bool InputManager::is_dragging() const
{
  return m_sdl_input_wrapper.is_dragging();
}

bool InputManager::has_dragged() const
{
  return m_sdl_input_wrapper.has_dragged();
}

const Vector4i& InputManager::get_drag_bounds() const
{
  return m_sdl_input_wrapper.get_drag_bounds();
}

const Vector2i& InputManager::get_mouse_position() const
{
  return m_sdl_input_wrapper.get_mouse_position();
}

const std::shared_ptr<InputContext> InputManager::get_current_context() const
{
  if (m_context_stack.empty())
  {
    spdlog::critical("No contexts avaiblable");
    return nullptr;
  }

  return m_context_stack.back();
}

bool InputManager::should_quit() const
{
  return m_sdl_input_wrapper.should_quit();
}

void InputManager::quit()
{
  m_sdl_input_wrapper.quit();
}

bool InputManager::window_size_changed() const
{
  return m_sdl_input_wrapper.window_size_changed();
}

void InputManager::text_input_start()
{
  m_sdl_input_wrapper.text_input_start();
}

void InputManager::text_input_stop()
{
  m_sdl_input_wrapper.text_input_stop();
}

const std::string& InputManager::get_text_input() const
{
  return m_sdl_input_wrapper.get_text_input();
}

int InputManager::get_text_input_cursor_index() const
{
  return m_sdl_input_wrapper.get_text_input_cursor_index();
}

void InputManager::set_text_input(const std::string& text)
{
  m_sdl_input_wrapper.set_text_input(text);
}

void InputManager::reset_drag()
{
  m_sdl_input_wrapper.reset_drag();
}

void InputManager::set_mouse_cursor(MouseCursor cursor)
{
  m_sdl_input_wrapper.set_mouse_cursor(cursor);
}

void InputManager::m_parse_input()
{
  JSON json{config::path::input};

  for (const auto& item : json.object.items())
  {
    const auto& key = entt::hashed_string{item.key().c_str()};
    const auto input_context = std::make_shared<InputContext>(key);

    for (const auto& value : item.value().items())
    {
      const auto& value_key = entt::hashed_string{value.key().c_str()};
      const std::vector<std::string>& values_strings = value.value();
      std::vector<uint32_t> values(values_strings.size(), 0);

      for (size_t i = 0; i < values_strings.size(); ++i)
      {
        values[i] = entt::hashed_string{values_strings[i].c_str()};
      }

      input_context->actions[value_key] = std::move(values);
      /* input_context->actions[value_key] = std::apply(entt::hashed_string{}, .get<std::vector<std::string>>()); */
    }

    m_available_contexts[key] = input_context;
  }
}
}  // namespace dl
