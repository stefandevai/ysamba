#include "./name_generator.hpp"

#include <spdlog/spdlog.h>

namespace dl
{
const std::filesystem::path NameGenerator::m_base_path = std::filesystem::path{"./data/world/name_generators"};

void NameGenerator::load(const std::string& key)
{
  if (!m_json.object.contains(key))
  {
    spdlog::critical("No name generator found with the key {}", key);
    return;
  }

  const auto& filepath = m_json.object[key].get<std::string>();
  m_weights.load(m_base_path / filepath);
}

std::string NameGenerator::generate()
{
  if (!m_weights.get_has_loaded())
  {
    spdlog::critical("The weights for the name generator are not loaded");
    return "";
  }

  return "";
}
}  // namespace dl
