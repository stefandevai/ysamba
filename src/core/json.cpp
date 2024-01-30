#include "./json.hpp"

#include <spdlog/spdlog.h>

#include "./utils.hpp"

namespace dl
{
JSON::JSON() {}
JSON::JSON(const std::string& filepath) { load(filepath); }

void JSON::load(const std::string& filepath)
{
  if (filepath.empty())
  {
    spdlog::critical("Filepath is empty.");
    return;
  }

  auto json_string = utils::read_file(filepath.c_str());
  object = nlohmann::json::parse(json_string);
  m_filepath = filepath;
  m_has_loaded = true;
}

void JSON::save(const std::string& filepath)
{
  if (filepath.empty())
  {
    spdlog::critical("Filepath is empty.");
    return;
  }

  std::string json_string;
  if (m_pretty_print)
  {
    json_string = object.dump(4);
  }
  else
  {
    json_string = object.dump();
  }

  utils::write_file(filepath, json_string);
  m_filepath = filepath;
}
}  // namespace dl
