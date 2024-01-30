#include "./utils.hpp"

#include <spdlog/spdlog.h>

#include <array>
#include <cstdint>
#include <fstream>
#include <random>

namespace
{
std::random_device dev;
std::mt19937 rng(dev());
}  // namespace

namespace dl::utils
{
std::string generate_id()
{
  std::uniform_int_distribution<std::size_t> dist(0, 15);
  const std::string values = "0123456789abcdef";

  const std::array<uint32_t, 16> has_dash = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};
  std::string result{};

  for (int i = 0; i < 16; i++)
  {
    if (has_dash[i] == 1)
    {
      result += "-";
    }

    result += values[dist(rng)];
    result += values[dist(rng)];
  }

  return result;
}

std::string read_file(const std::string& filepath)
{
  FILE* file = fopen(filepath.c_str(), "rt");

  if (file == nullptr)
  {
    spdlog::critical("Could not open file: {}", filepath);
    return "";
  }

  fseek(file, 0, SEEK_END);
  unsigned long length = ftell(file);
  char* data = new char[length + 1];
  memset(data, 0, length + 1);
  fseek(file, 0, SEEK_SET);
  fread(data, 1, length, file);
  fclose(file);

  std::string result(data);
  delete[] data;
  return result;
}

void write_file(const std::string& filepath, const std::string& content)
{
  std::ofstream outfile{filepath};

  if (!outfile.is_open())
  {
    spdlog::critical("Could not open file: {}", filepath);
    return;
  }

  outfile << content;
  outfile.close();
}
}  // namespace dl::utils
