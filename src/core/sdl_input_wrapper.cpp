#include <SDL.h>
#include <spdlog/spdlog.h>

#include "./input_manager.hpp"

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

  m_any_key_down = false;

  m_scroll.x = 0;
  m_scroll.y = 0;

  // Update key status
  SDL_Event event;

  while (SDL_PollEvent(&event))
  {
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
      break;
    }

    case SDL_MOUSEBUTTONDOWN:
    {
      if (event.button.button == SDL_BUTTON_LEFT)
      {
        m_mouse_state.first = true;
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        m_mouse_state.second = true;
      }
      break;
    }

    case SDL_MOUSEBUTTONUP:
    {
      if (event.button.button == SDL_BUTTON_LEFT)
      {
        m_mouse_state.first = false;
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        m_mouse_state.second = false;
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

    default:
      break;
    }
  }
}

bool SDLInputWrapper::is_any_key_down() { return m_any_key_down; }

bool SDLInputWrapper::is_key_down(const std::string& key)
{
  const auto it = m_key_down.find(m_key_map.at(key));

  if (it == m_key_down.end())
  {
    return false;
  }

  return it->second;
}

bool SDLInputWrapper::is_key_up(const std::string& key)
{
  const auto it = m_key_up.find(m_key_map.at(key));

  if (it == m_key_up.end())
  {
    return false;
  }

  return it->second;
}

bool SDLInputWrapper::should_quit() { return m_should_quit; }

void SDLInputWrapper::quit() { m_should_quit = true; }

const std::unordered_map<std::string, int> SDLInputWrapper::m_key_map = {
    {"k_backspace", SDLK_BACKSPACE},
    {"k_tab", SDLK_TAB},
    {"k_return", SDLK_RETURN},
    {"k_escape", SDLK_ESCAPE},
    {"k_space", SDLK_SPACE},
    {"k_exclaim", SDLK_EXCLAIM},
    {"k_quotedbl", SDLK_QUOTEDBL},
    {"k_hash", SDLK_HASH},
    {"k_dollar", SDLK_DOLLAR},
    {"k_percent", SDLK_PERCENT},
    {"k_ampersand", SDLK_AMPERSAND},
    {"k_quote", SDLK_QUOTE},
    {"k_leftparen", SDLK_LEFTPAREN},
    {"k_rightparen", SDLK_RIGHTPAREN},
    {"k_asterisk", SDLK_ASTERISK},
    {"k_plus", SDLK_PLUS},
    {"k_comma", SDLK_COMMA},
    {"k_minus", SDLK_MINUS},
    {"k_period", SDLK_PERIOD},
    {"k_slash", SDLK_SLASH},
    {"k_0", SDLK_0},
    {"k_1", SDLK_1},
    {"k_2", SDLK_2},
    {"k_3", SDLK_3},
    {"k_4", SDLK_4},
    {"k_5", SDLK_5},
    {"k_6", SDLK_6},
    {"k_7", SDLK_7},
    {"k_8", SDLK_8},
    {"k_9", SDLK_9},
    {"k_colon", SDLK_COLON},
    {"k_semicolon", SDLK_SEMICOLON},
    {"k_less", SDLK_LESS},
    {"k_equals", SDLK_EQUALS},
    {"k_greater", SDLK_GREATER},
    {"k_question", SDLK_QUESTION},
    {"k_at", SDLK_AT},
    {"k_leftbracket", SDLK_LEFTBRACKET},
    {"k_backslash", SDLK_BACKSLASH},
    {"k_rightbracket", SDLK_RIGHTBRACKET},
    {"k_caret", SDLK_CARET},
    {"k_underscore", SDLK_UNDERSCORE},
    {"k_backquote", SDLK_BACKQUOTE},
    {"k_a", SDLK_a},
    {"k_b", SDLK_b},
    {"k_c", SDLK_c},
    {"k_d", SDLK_d},
    {"k_e", SDLK_e},
    {"k_f", SDLK_f},
    {"k_g", SDLK_g},
    {"k_h", SDLK_h},
    {"k_i", SDLK_i},
    {"k_j", SDLK_j},
    {"k_k", SDLK_k},
    {"k_l", SDLK_l},
    {"k_m", SDLK_m},
    {"k_n", SDLK_n},
    {"k_o", SDLK_o},
    {"k_p", SDLK_p},
    {"k_q", SDLK_q},
    {"k_r", SDLK_r},
    {"k_s", SDLK_s},
    {"k_t", SDLK_t},
    {"k_u", SDLK_u},
    {"k_v", SDLK_v},
    {"k_w", SDLK_w},
    {"k_x", SDLK_x},
    {"k_y", SDLK_y},
    {"k_z", SDLK_z},
    {"k_delete", SDLK_DELETE},
    {"k_capslock", SDLK_CAPSLOCK},
    {"k_f1", SDLK_F1},
    {"k_f2", SDLK_F2},
    {"k_f3", SDLK_F3},
    {"k_f4", SDLK_F4},
    {"k_f5", SDLK_F5},
    {"k_f6", SDLK_F6},
    {"k_f7", SDLK_F7},
    {"k_f8", SDLK_F8},
    {"k_f9", SDLK_F9},
    {"k_f10", SDLK_F10},
    {"k_f11", SDLK_F11},
    {"k_f12", SDLK_F12},
    {"k_printscreen", SDLK_PRINTSCREEN},
    {"k_scrolllock", SDLK_SCROLLLOCK},
    {"k_pause", SDLK_PAUSE},
    {"k_insert", SDLK_INSERT},
    {"k_home", SDLK_HOME},
    {"k_pageup", SDLK_PAGEUP},
    {"k_end", SDLK_END},
    {"k_pagedown", SDLK_PAGEDOWN},
    {"k_right", SDLK_RIGHT},
    {"k_left", SDLK_LEFT},
    {"k_down", SDLK_DOWN},
    {"k_up", SDLK_UP},
    {"k_numlockclear", SDLK_NUMLOCKCLEAR},
    {"k_kp_divide", SDLK_KP_DIVIDE},
    {"k_kp_multiply", SDLK_KP_MULTIPLY},
    {"k_kp_minus", SDLK_KP_MINUS},
    {"k_kp_plus", SDLK_KP_PLUS},
    {"k_kp_enter", SDLK_KP_ENTER},
    {"k_kp_1", SDLK_KP_1},
    {"k_kp_2", SDLK_KP_2},
    {"k_kp_3", SDLK_KP_3},
    {"k_kp_4", SDLK_KP_4},
    {"k_kp_5", SDLK_KP_5},
    {"k_kp_6", SDLK_KP_6},
    {"k_kp_7", SDLK_KP_7},
    {"k_kp_8", SDLK_KP_8},
    {"k_kp_9", SDLK_KP_9},
    {"k_kp_0", SDLK_KP_0},
    {"k_kp_period", SDLK_KP_PERIOD},
    {"k_application", SDLK_APPLICATION},
    {"k_power", SDLK_POWER},
    {"k_kp_equals", SDLK_KP_EQUALS},
    {"k_f13", SDLK_F13},
    {"k_f14", SDLK_F14},
    {"k_f15", SDLK_F15},
    {"k_f16", SDLK_F16},
    {"k_f17", SDLK_F17},
    {"k_f18", SDLK_F18},
    {"k_f19", SDLK_F19},
    {"k_f20", SDLK_F20},
    {"k_f21", SDLK_F21},
    {"k_f22", SDLK_F22},
    {"k_f23", SDLK_F23},
    {"k_f24", SDLK_F24},
    {"k_execute", SDLK_EXECUTE},
    {"k_help", SDLK_HELP},
    {"k_menu", SDLK_MENU},
    {"k_select", SDLK_SELECT},
    {"k_stop", SDLK_STOP},
    {"k_again", SDLK_AGAIN},
    {"k_undo", SDLK_UNDO},
    {"k_cut", SDLK_CUT},
    {"k_copy", SDLK_COPY},
    {"k_paste", SDLK_PASTE},
    {"k_find", SDLK_FIND},
    {"k_mute", SDLK_MUTE},
    {"k_volumeup", SDLK_VOLUMEUP},
    {"k_volumedown", SDLK_VOLUMEDOWN},
    {"k_kp_comma", SDLK_KP_COMMA},
    {"k_kp_equalsas", SDLK_KP_EQUALSAS400},
    {"k_alterase", SDLK_ALTERASE},
    {"k_sysreq", SDLK_SYSREQ},
    {"k_cancel", SDLK_CANCEL},
    {"k_clear", SDLK_CLEAR},
    {"k_prior", SDLK_PRIOR},
    {"k_return2", SDLK_RETURN2},
    {"k_separator", SDLK_SEPARATOR},
    {"k_out", SDLK_OUT},
    {"k_oper", SDLK_OPER},
    {"k_clearagain", SDLK_CLEARAGAIN},
    {"k_crsel", SDLK_CRSEL},
    {"k_exsel", SDLK_EXSEL},
    {"k_kp_00", SDLK_KP_00},
    {"k_kp_000", SDLK_KP_000},
    {"k_thousandsseparator", SDLK_THOUSANDSSEPARATOR},
    {"k_decimalseparator", SDLK_DECIMALSEPARATOR},
    {"k_currencyunit", SDLK_CURRENCYUNIT},
    {"k_currencysubunit", SDLK_CURRENCYSUBUNIT},
    {"k_kp_leftparen", SDLK_KP_LEFTPAREN},
    {"k_kp_rightparen", SDLK_KP_RIGHTPAREN},
    {"k_kp_leftbrace", SDLK_KP_LEFTBRACE},
    {"k_kp_rightbrace", SDLK_KP_RIGHTBRACE},
    {"k_kp_tab", SDLK_KP_TAB},
    {"k_kp_backspace", SDLK_KP_BACKSPACE},
    {"k_kp_a", SDLK_KP_A},
    {"k_kp_b", SDLK_KP_B},
    {"k_kp_c", SDLK_KP_C},
    {"k_kp_d", SDLK_KP_D},
    {"k_kp_e", SDLK_KP_E},
    {"k_kp_f", SDLK_KP_F},
    {"k_kp_xor", SDLK_KP_XOR},
    {"k_kp_power", SDLK_KP_POWER},
    {"k_kp_percent", SDLK_KP_PERCENT},
    {"k_kp_less", SDLK_KP_LESS},
    {"k_kp_greater", SDLK_KP_GREATER},
    {"k_kp_ampersand", SDLK_KP_AMPERSAND},
    {"k_kp_dblampersand", SDLK_KP_DBLAMPERSAND},
    {"k_kp_verticalbar", SDLK_KP_VERTICALBAR},
    {"k_kp_dblverticalbar", SDLK_KP_DBLVERTICALBAR},
    {"k_kp_colon", SDLK_KP_COLON},
    {"k_kp_hash", SDLK_KP_HASH},
    {"k_kp_space", SDLK_KP_SPACE},
    {"k_kp_at", SDLK_KP_AT},
    {"k_kp_exclam", SDLK_KP_EXCLAM},
    {"k_kp_memstore", SDLK_KP_MEMSTORE},
    {"k_kp_memrecall", SDLK_KP_MEMRECALL},
    {"k_kp_memclear", SDLK_KP_MEMCLEAR},
    {"k_kp_memadd", SDLK_KP_MEMADD},
    {"k_kp_memsubtract", SDLK_KP_MEMSUBTRACT},
    {"k_kp_memmultiply", SDLK_KP_MEMMULTIPLY},
    {"k_kp_memdivide", SDLK_KP_MEMDIVIDE},
    {"k_kp_plusminus", SDLK_KP_PLUSMINUS},
    {"k_kp_clear", SDLK_KP_CLEAR},
    {"k_kp_clearentry", SDLK_KP_CLEARENTRY},
    {"k_kp_binary", SDLK_KP_BINARY},
    {"k_kp_octal", SDLK_KP_OCTAL},
    {"k_kp_decimal", SDLK_KP_DECIMAL},
    {"k_kp_hexadecimal", SDLK_KP_HEXADECIMAL},
    {"k_lctrl", SDLK_LCTRL},
    {"k_lshift", SDLK_LSHIFT},
    {"k_lalt", SDLK_LALT},
    {"k_lgui", SDLK_LGUI},
    {"k_rctrl", SDLK_RCTRL},
    {"k_rshift", SDLK_RSHIFT},
    {"k_ralt", SDLK_RALT},
    {"k_rgui", SDLK_RGUI},
    {"k_mode", SDLK_MODE},
    {"k_audionext", SDLK_AUDIONEXT},
    {"k_audioprev", SDLK_AUDIOPREV},
    {"k_audiostop", SDLK_AUDIOSTOP},
    {"k_audioplay", SDLK_AUDIOPLAY},
    {"k_audiomute", SDLK_AUDIOMUTE},
    {"k_mediaselect", SDLK_MEDIASELECT},
    {"k_www", SDLK_WWW},
    {"k_mail", SDLK_MAIL},
    {"k_calculator", SDLK_CALCULATOR},
    {"k_computer", SDLK_COMPUTER},
    {"k_ac_search", SDLK_AC_SEARCH},
    {"k_ac_home", SDLK_AC_HOME},
    {"k_ac_back", SDLK_AC_BACK},
    {"k_ac_forward", SDLK_AC_FORWARD},
    {"k_ac_stop", SDLK_AC_STOP},
    {"k_ac_refresh", SDLK_AC_REFRESH},
    {"k_ac_bookmarks", SDLK_AC_BOOKMARKS},
    {"k_brightnessdown", SDLK_BRIGHTNESSDOWN},
    {"k_brightnessup", SDLK_BRIGHTNESSUP},
    {"k_displayswitch", SDLK_DISPLAYSWITCH},
    {"k_kbdillumtoggle", SDLK_KBDILLUMTOGGLE},
    {"k_kbdillumdown", SDLK_KBDILLUMDOWN},
    {"k_kbdillumup", SDLK_KBDILLUMUP},
    {"k_eject", SDLK_EJECT},
    {"k_sleep", SDLK_SLEEP},
};
}  // namespace dl
