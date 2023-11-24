#include "./name_generator.hpp"

#include <spdlog/spdlog.h>

#include <map>
#include <unordered_map>

#include "../../core/random.hpp"

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

  const size_t max_tries = 5;
  const size_t max_internal_tries = 10;
  const size_t generation_limit = 23;
  const size_t min_length = 6;
  const size_t max_length = 12;
  std::string name{};

  for (size_t n = 0; n < max_tries; ++n)
  {
    name = random::get_weighted_value<std::string, double>(starts_keys, starts_values);
    auto candidates = grams.at(name);
    std::string next{};

    spdlog::info("TRY: {}", n);
    spdlog::info("NAME: {}", name);

    while (true)
    {
      auto current = random::get_weighted_value<std::string, double>(candidates.first, candidates.second);
      auto name_candidate = name + current;
      spdlog::debug("{}", name_candidate);
      /* name += current; */
      auto next_index = name_candidate.size() - order;
      next = name_candidate.substr(next_index, name_candidate.size());

      const auto next_in_grams = grams.contains(next);
      const auto next_in_ends = std::find(ends_keys.begin(), ends_keys.end(), next) != ends_keys.end();

      // Name is already built and has an ending
      if (name_candidate.size() > min_length && next_in_ends)
      {
        return name_candidate;
      }
      // Name is already bigger than the max length
      else if (name_candidate.size() > max_length - order)
      {
        // Try to get an end value and return a name
        for (size_t j = 0; j < max_internal_tries; ++j)
        {
          current = random::get_weighted_value<std::string, double>(candidates.first, candidates.second);
          if (std::find(ends_keys.begin(), ends_keys.end(), current) != ends_keys.end())
          {
            return name + current;
          }
        }

        spdlog::warn("BREAK MAX {}", name + current);
        // Retry name generation from start
        break;
      }
      else if (!next_in_grams)
      {
        // Retry name generation from start
        spdlog::warn("BREAK NEXT {} {}", next, name_candidate);
        break;
      }

      name = name_candidate;
      candidates = grams.at(next);
    }
  }

  spdlog::warn("Max tries exceeded for name generation: {}", name);

  return name;
}
}  // namespace dl

/* if (next_in_ends && !next_in_grams) */
/* { */
/*   const auto start = random::get_weighted_value<std::string, double>(starts_keys, starts_values); */
/*   name += ' ' + start; */
/*   next = start; */
/* } */
/* else if (!next_in_grams) */
/* { */
/*   const auto end = random::get_weighted_value<std::string, double>(ends_keys, ends_values); */
/*   const auto start = random::get_weighted_value<std::string, double>(starts_keys, starts_values); */

/*   name += end + ' ' + start; */
/*   next = start; */
/* } */
/* else if (next_in_ends) */
/* { */

/* } */
