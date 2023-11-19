#include "./lua_api.hpp"

#include <spdlog/spdlog.h>

#include "./file_manager.hpp"

namespace dl
{
LuaAPI::LuaAPI() { m_lua.open_libraries(sol::lib::base, sol::lib::package); }

LuaAPI::LuaAPI(const std::string& script_path)
{
  m_lua.open_libraries(sol::lib::base, sol::lib::package);
  load(script_path);
}

void LuaAPI::load(const std::string& script_path)
{
  try
  {
    auto script_full_path = FileManager::get_script_path(script_path);
    m_lua.script_file(script_full_path.u8string());
  }
  catch (const std::exception& exc)
  {
    spdlog::critical("{}", exc.what());
  }
}

void LuaAPI::run_function(const std::string& function_name) { m_lua[function_name](); }
}  // namespace dl
