#pragma once

#include <map>
#include <string>
#include <unordered_set>

#include "./society/job_type.hpp"

namespace dl
{
enum class Direction;

struct ActionItemResult
{
  uint32_t id{};
  uint32_t quantity{};
};

struct Action
{
  uint32_t id{};
  JobType type{};
  std::string label{};
  int turns_into = -1;
  bool gives_in_place = true;
  std::vector<std::string> qualities_required{};
  std::map<uint32_t, uint32_t> consumes{};
  std::vector<ActionItemResult> gives{};
};

struct TileData
{
  uint32_t id;
  std::string name;
  std::unordered_set<std::string> flags;
  std::unordered_map<JobType, Action> actions{};
  Direction climbs_to;
};
}  // namespace dl
