#include "./name_generator.hpp"

#include <spdlog/spdlog.h>

#include <map>
#include <unordered_map>

#include "core/random.hpp"

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

  using GramsMap = std::unordered_map<std::string, std::pair<std::vector<std::string>, std::vector<double>>>;

  const auto order = m_weights.object["order"].get<uint32_t>();
  const auto grams = m_weights.object["grams"].get<GramsMap>();
  const auto starts_keys = m_weights.object["starts_keys"].get<std::vector<std::string>>();
  const auto starts_values = m_weights.object["starts_values"].get<std::vector<double>>();
  const auto ends_keys = m_weights.object["ends_keys"].get<std::vector<std::string>>();
  const auto ends_values = m_weights.object["ends_values"].get<std::vector<double>>();

  std::string name{};

  for (size_t n = 0; n < m_max_tries; ++n)
  {
    const auto normal_length = random::get_normal_number<size_t>(m_min_length, m_max_length);
    const auto target_length = std::min(std::max(normal_length, m_min_length), m_max_length);
    name = random::get_weighted_value<std::string, double>(starts_keys, starts_values);
    auto candidates = grams.at(name);
    std::string next{};

    while (true)
    {
      auto current = random::get_weighted_value<std::string, double>(candidates.first, candidates.second);
      auto name_candidate = name + current;
      /* name += current; */
      auto next_index = name_candidate.size() - order;
      next = name_candidate.substr(next_index, name_candidate.size());

      const auto next_in_grams = grams.contains(next);
      const auto next_in_ends = std::find(ends_keys.begin(), ends_keys.end(), next) != ends_keys.end();

      // Name is already built and has an ending
      if (name_candidate.size() >= target_length && next_in_ends)
      {
        return name_candidate;
      }
      // Name is already bigger than the max length
      else if (name_candidate.size() >= m_max_length)
      {
        if (next_in_ends)
        {
          return name_candidate;
        }

        // Try to get an end value and return a name
        for (size_t j = 0; j < m_max_internal_tries; ++j)
        {
          current = random::get_weighted_value<std::string, double>(candidates.first, candidates.second);
          if (std::find(ends_keys.begin(), ends_keys.end(), current) != ends_keys.end())
          {
            return name + current;
          }
        }

        // Retry name generation from start
        break;
      }
      else if (!next_in_grams)
      {
        // Retry name generation from start
        break;
      }

      name = name_candidate;
      candidates = grams.at(next);
    }
  }

  return name;
}
}  // namespace dl
