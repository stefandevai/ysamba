#pragma once

#include <entt/entity/entity.hpp>
#include <map>
#include <vector>

#include "world/society/job_type.hpp"

// Forward declarations
namespace dl
{
struct Vector3i;
class World;
}  // namespace dl

namespace dl::utils
{
struct ValidateTileActionParams
{
  World& world;
  entt::registry& registry;
  const Vector3i& position;
  JobType job_type{};
};

struct HasQualitiesRequiredParams
{
  World& world;
  entt::registry& registry;
  entt::entity entity = entt::null;
  const std::vector<std::string>& qualities_required;
};

bool has_consumables(entt::registry& registry, const std::map<uint32_t, uint32_t>& consumables);

bool validate_tile_action(ValidateTileActionParams params);

bool has_qualities_required(HasQualitiesRequiredParams params);
}  // namespace dl::utils
