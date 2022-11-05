#include "./lua_api.hpp"
#include "./file_manager.hpp"

namespace dl
{
  LuaAPI::LuaAPI()
  {
    m_lua.open_libraries (sol::lib::base);
  }

  LuaAPI::LuaAPI (const std::string& script_path)
  {
    m_lua.open_libraries (sol::lib::base);
    load (script_path);
  }

  LuaAPI::~LuaAPI() {}

  void LuaAPI::load (const std::string& script_path)
  {
    try
    {
      auto script_full_path = FileManager::get_script_path(script_path);
      m_lua.script_file (script_full_path.u8string());
    }
    catch (const std::exception& exc)
    {
      std::cerr << exc.what() << "\n";
    }
  }

  void LuaAPI::run_function (const std::string& function_name)
  {
    m_lua[function_name]();
  }
}
