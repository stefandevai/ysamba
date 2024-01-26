#pragma once

#include <filesystem>

#include "core/json.hpp"

namespace dl
{
class NameGenerator
{
 public:
  NameGenerator(const size_t min_length = 4, const size_t max_length = 10)
      : m_min_length(min_length), m_max_length(max_length)
  {
  }

  void load(const std::string& key);
  [[nodiscard]] std::string generate();

 private:
  static const std::filesystem::path m_base_path;
  JSON m_json{(m_base_path / "index.json").string()};
  JSON m_weights;
  const size_t m_max_tries = 5;
  const size_t m_max_internal_tries = 10;
  const size_t m_min_length;
  const size_t m_max_length;
};
}  // namespace dl
