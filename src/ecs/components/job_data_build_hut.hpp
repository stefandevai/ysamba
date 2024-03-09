#pragma once

#include <cstdint>

namespace dl
{
struct JobDataBuildHut
{
  uint32_t hut_size;
};

template <typename Archive>
void serialize(Archive& archive, JobDataBuildHut& data)
{
  archive(data.hut_size);
}
}  // namespace dl
