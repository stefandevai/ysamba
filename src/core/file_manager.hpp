#pragma once

#include <filesystem>
#include <string>

namespace dl
{
  class FileManager
  {
    public:
      FileManager();

      // Removing copy-constructor and assignment operator
      FileManager(FileManager const&) {};
      void operator=(FileManager const&) {};

      static std::filesystem::path get_data_dir();
      static std::filesystem::path get_full_path(const std::string& relative_path);
      static std::filesystem::path get_script_path(const std::string& relative_path);
  };
}

