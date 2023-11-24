#include "./json.hpp"

#include <stdexcept>

#include "./fileutils.hpp"

namespace dl
{
JSON::JSON() {}
JSON::JSON(const std::string& filepath) { load(filepath); }

void JSON::load(const std::string& filepath)
{
  if (filepath.empty())
  {
    throw std::invalid_argument("Filepath is empty.");
  }

  auto json_string = FileUtils::read_file(filepath.c_str());
  object = nlohmann::json::parse(json_string);
  m_filepath = filepath;
  m_has_loaded = true;
}

void JSON::save(const std::string& filepath)
{
  if (filepath.empty())
  {
    throw std::invalid_argument("Filepath is empty.");
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

  FileUtils::write_file(json_string, filepath);
  m_filepath = filepath;
}
}  // namespace dl
