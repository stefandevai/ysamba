#pragma once

/* #include "../../../lib/json/json.hpp" */
#include <nlohmann/json.hpp>

namespace dl
{
class JSON
{
 public:
  nlohmann::json object;

 public:
  JSON();
  JSON(const std::string& filepath);

  void load(const std::string& filepath);
  void save(const std::string& filepath);
  void set_filepath(const std::string& filepath) { m_filepath = filepath; };
  void set_pretty_print(const bool status) { m_pretty_print = status; };
  std::string get_filepath() const { return m_filepath; };
  bool get_pretty_print() const { return m_pretty_print; };
  bool get_has_loaded() const { return m_has_loaded; }

 private:
  std::string m_filepath;
  bool m_pretty_print = true;
  bool m_has_loaded = false;
};
}  // namespace dl
