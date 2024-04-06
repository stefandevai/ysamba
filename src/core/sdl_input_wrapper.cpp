#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "./input_manager.hpp"
#include "definitions.hpp"

#ifdef DL_BUILD_DEBUG_TOOLS
#include "debug/debug_tools.hpp"
#endif

namespace
{
#ifdef DL_BUILD_DEBUG_TOOLS
auto& debug_tools = dl::DebugTools::get_instance();
#endif

using namespace entt::literals;
const std::unordered_map<uint32_t, int> key_map = {
    {"k_backspace"_hs, SDLK_BACKSPACE},
    {"k_tab"_hs, SDLK_TAB},
    {"k_return"_hs, SDLK_RETURN},
    {"k_escape"_hs, SDLK_ESCAPE},
    {"k_space"_hs, SDLK_SPACE},
    {"k_exclaim"_hs, SDLK_EXCLAIM},
    {"k_quotedbl"_hs, SDLK_QUOTEDBL},
    {"k_hash"_hs, SDLK_HASH},
    {"k_dollar"_hs, SDLK_DOLLAR},
    {"k_percent"_hs, SDLK_PERCENT},
    {"k_ampersand"_hs, SDLK_AMPERSAND},
    {"k_quote"_hs, SDLK_QUOTE},
    {"k_leftparen"_hs, SDLK_LEFTPAREN},
    {"k_rightparen"_hs, SDLK_RIGHTPAREN},
    {"k_asterisk"_hs, SDLK_ASTERISK},
    {"k_plus"_hs, SDLK_PLUS},
    {"k_comma"_hs, SDLK_COMMA},
    {"k_minus"_hs, SDLK_MINUS},
    {"k_period"_hs, SDLK_PERIOD},
    {"k_slash"_hs, SDLK_SLASH},
    {"k_0"_hs, SDLK_0},
    {"k_1"_hs, SDLK_1},
    {"k_2"_hs, SDLK_2},
    {"k_3"_hs, SDLK_3},
    {"k_4"_hs, SDLK_4},
    {"k_5"_hs, SDLK_5},
    {"k_6"_hs, SDLK_6},
    {"k_7"_hs, SDLK_7},
    {"k_8"_hs, SDLK_8},
    {"k_9"_hs, SDLK_9},
    {"k_colon"_hs, SDLK_COLON},
    {"k_semicolon"_hs, SDLK_SEMICOLON},
    {"k_less"_hs, SDLK_LESS},
    {"k_equals"_hs, SDLK_EQUALS},
    {"k_greater"_hs, SDLK_GREATER},
    {"k_question"_hs, SDLK_QUESTION},
    {"k_at"_hs, SDLK_AT},
    {"k_leftbracket"_hs, SDLK_LEFTBRACKET},
    {"k_backslash"_hs, SDLK_BACKSLASH},
    {"k_rightbracket"_hs, SDLK_RIGHTBRACKET},
    {"k_caret"_hs, SDLK_CARET},
    {"k_underscore"_hs, SDLK_UNDERSCORE},
    {"k_backquote"_hs, SDLK_BACKQUOTE},
    {"k_a"_hs, SDLK_a},
    {"k_b"_hs, SDLK_b},
    {"k_c"_hs, SDLK_c},
    {"k_d"_hs, SDLK_d},
    {"k_e"_hs, SDLK_e},
    {"k_f"_hs, SDLK_f},
    {"k_g"_hs, SDLK_g},
    {"k_h"_hs, SDLK_h},
    {"k_i"_hs, SDLK_i},
    {"k_j"_hs, SDLK_j},
    {"k_k"_hs, SDLK_k},
    {"k_l"_hs, SDLK_l},
    {"k_m"_hs, SDLK_m},
    {"k_n"_hs, SDLK_n},
    {"k_o"_hs, SDLK_o},
    {"k_p"_hs, SDLK_p},
    {"k_q"_hs, SDLK_q},
    {"k_r"_hs, SDLK_r},
    {"k_s"_hs, SDLK_s},
    {"k_t"_hs, SDLK_t},
    {"k_u"_hs, SDLK_u},
    {"k_v"_hs, SDLK_v},
    {"k_w"_hs, SDLK_w},
    {"k_x"_hs, SDLK_x},
    {"k_y"_hs, SDLK_y},
    {"k_z"_hs, SDLK_z},
    {"k_delete"_hs, SDLK_DELETE},
    {"k_capslock"_hs, SDLK_CAPSLOCK},
    {"k_f1"_hs, SDLK_F1},
    {"k_f2"_hs, SDLK_F2},
    {"k_f3"_hs, SDLK_F3},
    {"k_f4"_hs, SDLK_F4},
    {"k_f5"_hs, SDLK_F5},
    {"k_f6"_hs, SDLK_F6},
    {"k_f7"_hs, SDLK_F7},
    {"k_f8"_hs, SDLK_F8},
    {"k_f9"_hs, SDLK_F9},
    {"k_f10"_hs, SDLK_F10},
    {"k_f11"_hs, SDLK_F11},
    {"k_f12"_hs, SDLK_F12},
    {"k_printscreen"_hs, SDLK_PRINTSCREEN},
    {"k_scrolllock"_hs, SDLK_SCROLLLOCK},
    {"k_pause"_hs, SDLK_PAUSE},
    {"k_insert"_hs, SDLK_INSERT},
    {"k_home"_hs, SDLK_HOME},
    {"k_pageup"_hs, SDLK_PAGEUP},
    {"k_end"_hs, SDLK_END},
    {"k_pagedown"_hs, SDLK_PAGEDOWN},
    {"k_right"_hs, SDLK_RIGHT},
    {"k_left"_hs, SDLK_LEFT},
    {"k_down"_hs, SDLK_DOWN},
    {"k_up"_hs, SDLK_UP},
    {"k_numlockclear"_hs, SDLK_NUMLOCKCLEAR},
    {"k_kp_divide"_hs, SDLK_KP_DIVIDE},
    {"k_kp_multiply"_hs, SDLK_KP_MULTIPLY},
    {"k_kp_minus"_hs, SDLK_KP_MINUS},
    {"k_kp_plus"_hs, SDLK_KP_PLUS},
    {"k_kp_enter"_hs, SDLK_KP_ENTER},
    {"k_kp_1"_hs, SDLK_KP_1},
    {"k_kp_2"_hs, SDLK_KP_2},
    {"k_kp_3"_hs, SDLK_KP_3},
    {"k_kp_4"_hs, SDLK_KP_4},
    {"k_kp_5"_hs, SDLK_KP_5},
    {"k_kp_6"_hs, SDLK_KP_6},
    {"k_kp_7"_hs, SDLK_KP_7},
    {"k_kp_8"_hs, SDLK_KP_8},
    {"k_kp_9"_hs, SDLK_KP_9},
    {"k_kp_0"_hs, SDLK_KP_0},
    {"k_kp_period"_hs, SDLK_KP_PERIOD},
    {"k_application"_hs, SDLK_APPLICATION},
    {"k_power"_hs, SDLK_POWER},
    {"k_kp_equals"_hs, SDLK_KP_EQUALS},
    {"k_f13"_hs, SDLK_F13},
    {"k_f14"_hs, SDLK_F14},
    {"k_f15"_hs, SDLK_F15},
    {"k_f16"_hs, SDLK_F16},
    {"k_f17"_hs, SDLK_F17},
    {"k_f18"_hs, SDLK_F18},
    {"k_f19"_hs, SDLK_F19},
    {"k_f20"_hs, SDLK_F20},
    {"k_f21"_hs, SDLK_F21},
    {"k_f22"_hs, SDLK_F22},
    {"k_f23"_hs, SDLK_F23},
    {"k_f24"_hs, SDLK_F24},
    {"k_execute"_hs, SDLK_EXECUTE},
    {"k_help"_hs, SDLK_HELP},
    {"k_menu"_hs, SDLK_MENU},
    {"k_select"_hs, SDLK_SELECT},
    {"k_stop"_hs, SDLK_STOP},
    {"k_again"_hs, SDLK_AGAIN},
    {"k_undo"_hs, SDLK_UNDO},
    {"k_cut"_hs, SDLK_CUT},
    {"k_copy"_hs, SDLK_COPY},
    {"k_paste"_hs, SDLK_PASTE},
    {"k_find"_hs, SDLK_FIND},
    {"k_mute"_hs, SDLK_MUTE},
    {"k_volumeup"_hs, SDLK_VOLUMEUP},
    {"k_volumedown"_hs, SDLK_VOLUMEDOWN},
    {"k_kp_comma"_hs, SDLK_KP_COMMA},
    {"k_kp_equalsas"_hs, SDLK_KP_EQUALSAS400},
    {"k_alterase"_hs, SDLK_ALTERASE},
    {"k_sysreq"_hs, SDLK_SYSREQ},
    {"k_cancel"_hs, SDLK_CANCEL},
    {"k_clear"_hs, SDLK_CLEAR},
    {"k_prior"_hs, SDLK_PRIOR},
    {"k_return2"_hs, SDLK_RETURN2},
    {"k_separator"_hs, SDLK_SEPARATOR},
    {"k_out"_hs, SDLK_OUT},
    {"k_oper"_hs, SDLK_OPER},
    {"k_clearagain"_hs, SDLK_CLEARAGAIN},
    {"k_crsel"_hs, SDLK_CRSEL},
    {"k_exsel"_hs, SDLK_EXSEL},
    {"k_kp_00"_hs, SDLK_KP_00},
    {"k_kp_000"_hs, SDLK_KP_000},
    {"k_thousandsseparator"_hs, SDLK_THOUSANDSSEPARATOR},
    {"k_decimalseparator"_hs, SDLK_DECIMALSEPARATOR},
    {"k_currencyunit"_hs, SDLK_CURRENCYUNIT},
    {"k_currencysubunit"_hs, SDLK_CURRENCYSUBUNIT},
    {"k_kp_leftparen"_hs, SDLK_KP_LEFTPAREN},
    {"k_kp_rightparen"_hs, SDLK_KP_RIGHTPAREN},
    {"k_kp_leftbrace"_hs, SDLK_KP_LEFTBRACE},
    {"k_kp_rightbrace"_hs, SDLK_KP_RIGHTBRACE},
    {"k_kp_tab"_hs, SDLK_KP_TAB},
    {"k_kp_backspace"_hs, SDLK_KP_BACKSPACE},
    {"k_kp_a"_hs, SDLK_KP_A},
    {"k_kp_b"_hs, SDLK_KP_B},
    {"k_kp_c"_hs, SDLK_KP_C},
    {"k_kp_d"_hs, SDLK_KP_D},
    {"k_kp_e"_hs, SDLK_KP_E},
    {"k_kp_f"_hs, SDLK_KP_F},
    {"k_kp_xor"_hs, SDLK_KP_XOR},
    {"k_kp_power"_hs, SDLK_KP_POWER},
    {"k_kp_percent"_hs, SDLK_KP_PERCENT},
    {"k_kp_less"_hs, SDLK_KP_LESS},
    {"k_kp_greater"_hs, SDLK_KP_GREATER},
    {"k_kp_ampersand"_hs, SDLK_KP_AMPERSAND},
    {"k_kp_dblampersand"_hs, SDLK_KP_DBLAMPERSAND},
    {"k_kp_verticalbar"_hs, SDLK_KP_VERTICALBAR},
    {"k_kp_dblverticalbar"_hs, SDLK_KP_DBLVERTICALBAR},
    {"k_kp_colon"_hs, SDLK_KP_COLON},
    {"k_kp_hash"_hs, SDLK_KP_HASH},
    {"k_kp_space"_hs, SDLK_KP_SPACE},
    {"k_kp_at"_hs, SDLK_KP_AT},
    {"k_kp_exclam"_hs, SDLK_KP_EXCLAM},
    {"k_kp_memstore"_hs, SDLK_KP_MEMSTORE},
    {"k_kp_memrecall"_hs, SDLK_KP_MEMRECALL},
    {"k_kp_memclear"_hs, SDLK_KP_MEMCLEAR},
    {"k_kp_memadd"_hs, SDLK_KP_MEMADD},
    {"k_kp_memsubtract"_hs, SDLK_KP_MEMSUBTRACT},
    {"k_kp_memmultiply"_hs, SDLK_KP_MEMMULTIPLY},
    {"k_kp_memdivide"_hs, SDLK_KP_MEMDIVIDE},
    {"k_kp_plusminus"_hs, SDLK_KP_PLUSMINUS},
    {"k_kp_clear"_hs, SDLK_KP_CLEAR},
    {"k_kp_clearentry"_hs, SDLK_KP_CLEARENTRY},
    {"k_kp_binary"_hs, SDLK_KP_BINARY},
    {"k_kp_octal"_hs, SDLK_KP_OCTAL},
    {"k_kp_decimal"_hs, SDLK_KP_DECIMAL},
    {"k_kp_hexadecimal"_hs, SDLK_KP_HEXADECIMAL},
    {"k_lctrl"_hs, SDLK_LCTRL},
    {"k_lshift"_hs, SDLK_LSHIFT},
    {"k_lalt"_hs, SDLK_LALT},
    {"k_lgui"_hs, SDLK_LGUI},
    {"k_rctrl"_hs, SDLK_RCTRL},
    {"k_rshift"_hs, SDLK_RSHIFT},
    {"k_ralt"_hs, SDLK_RALT},
    {"k_rgui"_hs, SDLK_RGUI},
    {"k_mode"_hs, SDLK_MODE},
    {"k_audionext"_hs, SDLK_AUDIONEXT},
    {"k_audioprev"_hs, SDLK_AUDIOPREV},
    {"k_audiostop"_hs, SDLK_AUDIOSTOP},
    {"k_audioplay"_hs, SDLK_AUDIOPLAY},
    {"k_audiomute"_hs, SDLK_AUDIOMUTE},
    {"k_mediaselect"_hs, SDLK_MEDIASELECT},
    {"k_www"_hs, SDLK_WWW},
    {"k_mail"_hs, SDLK_MAIL},
    {"k_calculator"_hs, SDLK_CALCULATOR},
    {"k_computer"_hs, SDLK_COMPUTER},
    {"k_ac_search"_hs, SDLK_AC_SEARCH},
    {"k_ac_home"_hs, SDLK_AC_HOME},
    {"k_ac_back"_hs, SDLK_AC_BACK},
    {"k_ac_forward"_hs, SDLK_AC_FORWARD},
    {"k_ac_stop"_hs, SDLK_AC_STOP},
    {"k_ac_refresh"_hs, SDLK_AC_REFRESH},
    {"k_ac_bookmarks"_hs, SDLK_AC_BOOKMARKS},
    {"k_brightnessdown"_hs, SDLK_BRIGHTNESSDOWN},
    {"k_brightnessup"_hs, SDLK_BRIGHTNESSUP},
    {"k_displayswitch"_hs, SDLK_DISPLAYSWITCH},
    {"k_kbdillumtoggle"_hs, SDLK_KBDILLUMTOGGLE},
    {"k_kbdillumdown"_hs, SDLK_KBDILLUMDOWN},
    {"k_kbdillumup"_hs, SDLK_KBDILLUMUP},
    {"k_eject"_hs, SDLK_EJECT},
    {"k_sleep"_hs, SDLK_SLEEP},
};
}  // namespace

namespace dl
{
SDLInputWrapper::SDLInputWrapper() {}

void SDLInputWrapper::update()
{
  // Block until events exist
  /* SDL_WaitEvent(nullptr); */

  // Reset keys
  for (auto& it : m_key_down)
  {
    it.second = false;
  }

  for (auto& it : m_key_up)
  {
    it.second = false;
  }

  m_window_size_changed = false;

  m_any_key_down = false;

  m_scroll.x = 0;
  m_scroll.y = 0;

  m_mouse_state_up.first = false;
  m_mouse_state_up.second = false;
  m_has_dragged = false;

  SDL_Event event;

  while (SDL_PollEvent(&event))
  {
#ifdef DL_BUILD_DEBUG_TOOLS
    debug_tools.process_event(&event);
#endif

    switch (event.type)
    {
    case SDL_QUIT:
    {
      m_should_quit = true;
      break;
    }

    case SDL_KEYDOWN:
    {
      const int index = event.key.keysym.sym;

      m_key_down[index] = true;
      m_any_key_down = true;

#ifdef DL_BUILD_DEBUG_TOOLS
      if (event.key.keysym.sym == SDLK_F3)
      {
        debug_tools.open = !debug_tools.open;
      }
#endif
      break;
    }

    case SDL_KEYUP:
    {
      const int index = event.key.keysym.sym;
      m_key_up[index] = true;
      break;
    }

    case SDL_MOUSEMOTION:
    {
      m_mouse_position.x = event.motion.x;
      m_mouse_position.y = event.motion.y;

      if (m_is_dragging)
      {
        m_drag_bounds.z = m_mouse_position.x;
        m_drag_bounds.w = m_mouse_position.y;
      }
      else if (m_mouse_state_down.first || m_mouse_state_down.second)
      {
        m_is_dragging = true;
        m_drag_bounds.x = m_mouse_position.x;
        m_drag_bounds.y = m_mouse_position.y;
        m_drag_bounds.z = m_mouse_position.x;
        m_drag_bounds.w = m_mouse_position.y;
        m_mouse_state_down.first = false;
        m_mouse_state_down.second = false;
      }
      break;
    }

    case SDL_MOUSEBUTTONDOWN:
    {
      if (!m_mouse_down_timer.is_running)
      {
        m_mouse_down_timer.start();
      }

      if (!m_is_dragging)
      {
        // Enter dragging mode if mouse is held down for 200ms
        if (m_mouse_down_timer.elapsed<std::chrono::milliseconds>() > 200)
        {
          m_is_dragging = true;
          m_drag_bounds.x = m_mouse_position.x;
          m_drag_bounds.y = m_mouse_position.y;
          m_drag_bounds.z = m_mouse_position.x;
          m_drag_bounds.w = m_mouse_position.y;
          m_mouse_state_down.first = false;
          m_mouse_state_down.second = false;
        }
        else if (event.button.button == SDL_BUTTON_LEFT)
        {
          m_mouse_state_down.first = true;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
          m_mouse_state_down.second = true;
        }
      }
      break;
    }

    case SDL_MOUSEBUTTONUP:
    {
      m_mouse_down_timer.stop();

      if (!m_is_dragging)
      {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          m_mouse_state_up.first = true;
          m_mouse_state_down.first = false;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
          m_mouse_state_up.second = true;
          m_mouse_state_down.second = false;
        }
      }
      else
      {
        m_is_dragging = false;
        m_has_dragged = true;
      }
      break;
    }

    case SDL_MOUSEWHEEL:
    {
      m_scroll.x = event.wheel.x;
      m_scroll.y = event.wheel.y;
      break;
    }

    case SDL_WINDOWEVENT:
    {
      switch (event.window.event)
      {
      case SDL_WINDOWEVENT_RESIZED:
      case SDL_WINDOWEVENT_SIZE_CHANGED:
        m_window_size_changed = true;
        break;
      }
      break;
    }

    case SDL_TEXTINPUT:
    {
      if (m_capture_text_input)
      {
        const std::string input_text{event.text.text};
        const auto index = std::distance(std::as_const(m_text_input).begin(), m_cursor.string_iterator);
        m_text_input.insert(m_cursor.string_iterator, input_text.begin(), input_text.end());
        m_cursor = m_text_input.begin() + index;
        ++m_cursor;
      }
      break;
    }

    default:
      break;
    }
  }

  if (m_capture_text_input)
  {
    if (m_key_down[SDLK_BACKSPACE] && !m_text_input.empty())
    {
      if (m_cursor.string_iterator != m_text_input.begin())
      {
        const auto old_position = m_cursor.string_iterator;
        --m_cursor;
        const auto index = std::distance(std::as_const(m_text_input).begin(), m_cursor.string_iterator);
        m_text_input.erase(m_cursor.string_iterator, old_position);
        m_cursor = m_text_input.begin() + index;
      }
    }
    else if (m_key_down[SDLK_v] && SDL_GetModState() & KMOD_CTRL)
    {
      char* clipboard_text = SDL_GetClipboardText();
      const std::string clipboard_string{clipboard_text};
      SDL_free(clipboard_text);

      const auto index = std::distance(std::as_const(m_text_input).begin(), m_cursor.string_iterator);
      m_text_input.insert(m_cursor.string_iterator, clipboard_string.begin(), clipboard_string.end());
      m_cursor = m_text_input.begin() + index;
      m_cursor += clipboard_string.size();
    }
    else if (m_key_down[SDLK_LEFT])
    {
      if (m_cursor.string_iterator != m_text_input.begin())
      {
        --m_cursor;
      }
    }
    else if (m_key_down[SDLK_RIGHT])
    {
      if (m_cursor.string_iterator != m_text_input.end())
      {
        ++m_cursor;
      }
    }
  }

#ifdef DL_BUILD_DEBUG_TOOLS
  // debug_tools.update();
#endif
}

bool SDLInputWrapper::is_any_key_down() const
{
  return m_any_key_down;
}

bool SDLInputWrapper::is_key_down(const uint32_t key)
{
  return m_key_down[key_map.at(key)];
}

bool SDLInputWrapper::is_key_up(const uint32_t key)
{
  return m_key_up[key_map.at(key)];
}

bool SDLInputWrapper::should_quit() const
{
  return m_should_quit;
}

void SDLInputWrapper::quit()
{
  m_should_quit = true;
}

void SDLInputWrapper::text_input_start()
{
  SDL_StartTextInput();
  m_capture_text_input = true;
  m_cursor = m_text_input.end();
}

void SDLInputWrapper::text_input_stop()
{
  m_capture_text_input = false;
  SDL_StopTextInput();
  m_text_input.clear();
  m_cursor = m_text_input.end();
}

const std::string& SDLInputWrapper::get_text_input() const
{
  return m_text_input;
}

int SDLInputWrapper::get_text_input_cursor_index() const
{
  if (!m_capture_text_input)
  {
    return 0;
  }

  return std::distance(UTF8Iterator(m_text_input.begin()), m_cursor);
}

void SDLInputWrapper::set_text_input(const std::string& text)
{
  m_text_input = text;
  m_cursor = m_text_input.end();
}
}  // namespace dl
