#pragma once

#define SOL_CHECK_ARGUMENTS 1
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp> // IWYU pragma: export

namespace dl
{
  class LuaAPI
  {
  protected:
    sol::state m_lua;

  public:
    LuaAPI();
    LuaAPI (const std::string& script_path);

    void load (const std::string& script_path);
    void run_function (const std::string& function_name);

    template<typename... Params>
    inline void run_function (const std::string& function_name, const Params&... params)
    {
      m_lua[function_name](params...);
    }

    template<typename T>
    inline void set_function (const std::string& function_name, T a_function)
    {
      m_lua.set_function (function_name, a_function);
    }

    template<typename T, typename R>
    inline void set_function (const std::string& function_name, T a_function, R a_instance)
    {
      m_lua.set_function (function_name, a_function, a_instance);
    }

    template<typename T>
    inline void set_variable (const std::string& name, T value)
    {
      m_lua[name] = value;
    }

    template<typename T>
    inline T get_variable (const std::string& name)
    {
      return m_lua.get<T> (name);
    }

    template<typename T>
    inline sol::optional<T> get_optional_variable (const std::string& name)
    {
      return m_lua.get<sol::optional<T>> (name);
    }
  };
} // namespace dl
