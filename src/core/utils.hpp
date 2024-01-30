#pragma once

#include <string>

namespace dl::utils
{
std::string generate_id();
std::string read_file(const std::string& filepath);
void write_file(const std::string& filepath, const std::string& content);
}  // namespace dl::utils
