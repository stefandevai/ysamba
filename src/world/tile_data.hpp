#pragma once

#include <map>
#include <string>
#include <unordered_set>

#include "./society/job_type.hpp"

namespace dl
{
struct Action
{
  std::string name{};
  uint32_t turns_into{};
  bool gives_in_place = true;
  std::vector<std::string> qualities_required{};
  std::map<uint32_t, uint32_t> consumes{};
  std::map<uint32_t, std::pair<uint32_t, uint32_t>> gives{};
};

struct TileData
{
  uint32_t id;
  std::string name;
  std::unordered_set<std::string> flags;
  std::unordered_map<JobType, Action> actions{};
  int climbs_to = -1;
};
}  // namespace dl
