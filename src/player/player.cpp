#include <libtcod.hpp>
#include "./player.hpp"
#include "../core/input_manager.hpp"

namespace dl
{
  static auto input_manager = InputManager::get_instance();

  Player::Player()
  {
    m_load();
  }

  Player::~Player() {  }

  void Player::update(const uint32_t delta)
  {
    m_should_advance_turn = false;

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

  void Player::m_load()
  {
    body.position.x = 10;
    body.position.y = 10;
    body.position.z = 0;
  }

  void Player::m_move()
  {
    if (input_manager->is_action_down("move_right"))
    {
      body.velocity.x = 1;
      m_should_advance_turn = true;
    }
    else if (input_manager->is_action_down("move_left"))
    {
      body.velocity.x = -1;
      m_should_advance_turn = true;
    }
    else if (input_manager->is_action_down("move_bottom"))
    {
      body.velocity.y = 1;
      m_should_advance_turn = true;
    }
    else if (input_manager->is_action_down("move_top"))
    {
      body.velocity.y = -1;
      m_should_advance_turn = true;
    }
    else if (input_manager->is_action_down("move_bottom_left"))
    {
      body.velocity.x = -1;
      body.velocity.y = 1;
      m_should_advance_turn = true;
    }
    else if (input_manager->is_action_down("move_bottom_right"))
    {
      body.velocity.x = 1;
      body.velocity.y = 1;
      m_should_advance_turn = true;
    }
    else if (input_manager->is_action_down("move_top_left"))
    {
      body.velocity.x = -1;
      body.velocity.y = -1;
      m_should_advance_turn = true;
    }
    else if (input_manager->is_action_down("move_top_right"))
    {
      body.velocity.x = 1;
      body.velocity.y = -1;
      m_should_advance_turn = true;
    }
  }

  void Player::render(TCOD_Console& console)
  {
    tcod::print(console, {body.position.x, body.position.y}, "@", TCOD_white, std::nullopt);
  }
}
