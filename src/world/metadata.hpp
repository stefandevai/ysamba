#pragma once

#include <chrono>
#include <string>

#include "core/maths/vector.hpp"

namespace dl
{
struct WorldMetadata
{
  std::string id{};
  std::string name{};
  int seed{};
  Vector3i world_size{};
  std::string created_at_label{};
  std::string updated_at_label{};
  std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> created_at{};
  std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> updated_at{};
};

template <typename Archive>
void serialize(Archive& archive, WorldMetadata& data)
{
  archive(data.id, data.name, data.seed, data.world_size, data.created_at_label, data.updated_at_label);
}

}  // namespace dl
