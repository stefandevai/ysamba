#include "./file_manager.hpp"

#include <spdlog/spdlog.h>

namespace dl
{
FileManager::FileManager() {}

std::filesystem::path FileManager::get_data_dir()
{
  static auto root_directory = std::filesystem::path{"."};  // Begin at the working directory.
  while (!std::filesystem::exists(root_directory / "data"))
  {
    // If the current working directory is missing the data dir then it will assume it exists in any parent directory.
    root_directory /= "..";
    if (!std::filesystem::exists(root_directory))
    {
      throw std::runtime_error("Could not find the data directory.");
    }
  }
  return root_directory / "data";
}

std::filesystem::path FileManager::get_full_path(const std::string& relative_path)
{
  const auto potential_path = FileManager::get_data_dir() / relative_path;

  if (!std::filesystem::exists(potential_path))
  {
    spdlog::critical("The path does not exist: {}", potential_path.string());
    throw std::runtime_error("The path does not exist");
  }

  return potential_path;
}

std::filesystem::path FileManager::get_script_path(const std::string& relative_path)
{
  return get_full_path((std::filesystem::path{"scripts"} / relative_path).string());
}
}  // namespace dl
