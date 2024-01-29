#pragma once

#include <string>

namespace dl
{
struct WorldMetadata
{
  std::string id{};
  std::string name{};
  int seed{};
};

template <typename Archive>
void serialize(Archive& archive, WorldMetadata& data)
{
  archive(data.id, data.name, data.seed);
}
}  // namespace dl
