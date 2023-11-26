#include "./input_manager.hpp"

#include <SDL.h>
#include <spdlog/spdlog.h>

#include <vector>

namespace dl
{
std::shared_ptr<InputManager> InputManager::m_instance = nullptr;
SDLInputWrapper InputManager::m_sdl_input_wrapper = SDLInputWrapper();
std::unordered_map<std::string, std::shared_ptr<InputContext>> InputManager::m_available_contexts = {};
std::vector<std::shared_ptr<InputContext>> InputManager::m_context_stack = {};

InputManager::InputManager() { m_parse_input(); }

std::shared_ptr<InputManager> InputManager::get_instance()
{
  if (m_instance == nullptr)
  {
    m_instance = std::make_shared<InputManager>();
  }

  return m_instance;
}

void InputManager::update() { m_sdl_input_wrapper.update(); }

void InputManager::push_context(const std::string& context_key)
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

bool InputManager::poll_action(const std::string& action_key)
{
  if (m_context_stack.empty())
  {
    spdlog::critical("No contexts avaiblable");
    return false;
  }

  const auto current_context = m_context_stack.back();

  if (!current_context->actions.contains(action_key))
  {
    spdlog::critical("Could not find action in this context: {}", action_key);
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

bool InputManager::is_key_down(const std::string& key) { return m_sdl_input_wrapper.is_key_down(key); }

bool InputManager::is_any_key_down() { return m_sdl_input_wrapper.is_any_key_down(); }

bool InputManager::is_key_up(const std::string& key) { return m_sdl_input_wrapper.is_key_up(key); }

bool InputManager::is_clicking(const MouseButton button)
{
  const auto& mouse_state = m_sdl_input_wrapper.get_mouse_state();

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

const Vector2i& InputManager::get_mouse_position() { return m_sdl_input_wrapper.get_mouse_position(); }

bool InputManager::should_quit() { return m_sdl_input_wrapper.should_quit(); }

void InputManager::quit() { m_sdl_input_wrapper.quit(); }

bool InputManager::window_size_changed() const { return m_sdl_input_wrapper.window_size_changed(); }

void InputManager::set_window_size_changed(const bool value) { m_sdl_input_wrapper.set_window_size_changed(value); }

void InputManager::m_parse_input()
{
  for (const auto& item : m_json.object.items())
  {
    const auto& key = item.key();
    const auto input_context = std::make_shared<InputContext>();

    for (const auto& value : item.value().items())
    {
      const auto& value_key = value.key();
      input_context->actions[value_key] = value.value();
    }

    m_available_contexts[key] = input_context;
  }
}
}  // namespace dl
