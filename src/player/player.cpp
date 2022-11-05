#include <libtcod.hpp>
#include "./player.hpp"
#include "../core/input_manager.hpp"


namespace dl
{
  static auto input_manager = InputManager::get_instance();

  void Player::update(const uint32_t delta)
  {
    if (m_delta_since_move < 50)
    {
      m_delta_since_move += delta;
    }
    else
    {
      m_delta_since_move = 0;
      m_move();
    }
  }

  void Player::m_move()
  {
    if (input_manager->is_action_down("move_right"))
    {
      m_position.x += 1;
    }
    else if (input_manager->is_action_down("move_left"))
    {
      m_position.x -= 1;
    }
    else if (input_manager->is_action_down("move_bottom"))
    {
      m_position.y += 1;
    }
    else if (input_manager->is_action_down("move_top"))
    {
      m_position.y -= 1;
    }
    else if (input_manager->is_action_down("move_bottom_left"))
    {
      m_position.x -= 1;
      m_position.y += 1;
    }
    else if (input_manager->is_action_down("move_bottom_right"))
    {
      m_position.x += 1;
      m_position.y += 1;
    }
    else if (input_manager->is_action_down("move_top_left"))
    {
      m_position.x -= 1;
      m_position.y -= 1;
    }
    else if (input_manager->is_action_down("move_top_right"))
    {
      m_position.x += 1;
      m_position.y -= 1;
    }
  }

  void Player::render(TCOD_Console& console)
  {
    tcod::print(console, {m_position.x, m_position.y}, "@", TCOD_white, std::nullopt);
  }
}

