#pragma once

#include <array>
#include <glm/vec2.hpp>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "core/maths/vector.hpp"
#include "graphics/renderer/texture.hpp"

namespace dl
{
struct NinePatchData
{
  const uint32_t top{};
  const uint32_t left{};
  const uint32_t bottom{};
  const uint32_t right{};
  const uint32_t border{};
};

struct SliceData
{
  std::array<glm::vec2, 4> uv_coordinates{};
};

using FreeformFrameData = std::variant<NinePatchData, SliceData>;

class TextureAtlas
{
 public:
  bool has_loaded = false;
  bool has_metadata = false;

  std::unique_ptr<Texture> texture = nullptr;

  TextureAtlas(const std::string& filepath, const std::string& data_filepath);

  // Loads after setting filepath
  void load(WGPUDevice device);

  [[nodiscard]] inline const Vector2i& get_size() const { return texture->size; }

  // Get top-left, top-right, bottom-right and bottom-left uv coordinates
  [[nodiscard]] const std::array<glm::vec2, 4>& get_uv_coordinates(uint32_t frame = 0) const;

  template <typename T>
  [[nodiscard]] const T& get_metadata(const uint32_t game_id) const
  {
    return std::get<T>(m_frame_data.at(game_id));
  }

 private:
  std::string m_data_filepath{};

  // Map where the key is the frame id and the value is an array of uv coordinates for a quad
  std::unordered_map<uint32_t, const std::array<glm::vec2, 4>> m_uv_coordinates{};
  std::unordered_map<uint32_t, FreeformFrameData> m_frame_data{};

  // Load texture metadata from a json file
  void m_load_metadata(const std::string& filepath);
};
}  // namespace dl
