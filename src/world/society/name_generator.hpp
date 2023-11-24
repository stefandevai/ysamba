#pragma once

#include <filesystem>

#include "../../core/json.hpp"

namespace dl
{
class NameGenerator
{
 public:
  NameGenerator() {}

  void load(const std::string& key);
  std::string generate();
  void set_seed(const int seed) { m_seed = seed; }

 private:
  static const std::filesystem::path m_base_path;

  JSON m_json{m_base_path / "index.json"};
  JSON m_weights;
  int m_seed = 0;
};
}  // namespace dl
