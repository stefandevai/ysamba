#pragma once

#include <filesystem>

#include "core/maths/vector.hpp"

namespace dl::config
{
const std::filesystem::path data_directory{"dl_data"};
const std::filesystem::path chunks_directory{"chunks"};
const Vector3i chunk_size{128, 128, 10};
}  // namespace dl::config
