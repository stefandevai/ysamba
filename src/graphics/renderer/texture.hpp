#pragma once

#include <webgpu/wgpu.h>

#include <array>
#include <glm/vec2.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "graphics/frame_angle.hpp"

namespace dl
{
enum class SpriteType
{
  Single,
  Multiple,
};

struct FrameData
{
  uint32_t frame = 0;
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t pattern_width = 0;
  uint32_t pattern_height = 0;
  int anchor_x = 0;
  int anchor_y = 0;
  uint32_t front_face_id = 0;
  FrameAngle angle = FrameAngle::Parallel;
  SpriteType sprite_type = SpriteType::Single;
  std::vector<uint32_t> pattern{};
};

class Texture
{
 public:
  bool has_loaded = false;

  WGPUTexture texture;
  WGPUTextureView view;

  // Create single texture
  Texture(const std::string& filepath);
  // Create uniform texture atlas
  Texture(const std::string& filepath,
          const int horizontal_frames,
          const int vertical_frames,
          const std::string& data_filepath = "");
  // Create empty texture in order to load it later
  Texture(const int width, const int height);
  // Create texture providing raw data
  Texture(const std::vector<unsigned char>& data, const int width, const int height);
  ~Texture();

  static Texture dummy(const WGPUDevice device);

  void load(const WGPUDevice device);
  void load(const WGPUDevice device, const unsigned char* data, const int width, const int height, const int channels);
  inline unsigned int get_id() const { return m_id; }
  inline int get_width() const { return m_width; }
  inline int get_height() const { return m_height; }
  inline int get_horizontal_frames() const { return m_horizontal_frames; }
  inline int get_vertical_frames() const { return m_vertical_frames; }
  // TODO: Implement irregular frame calculations
  float get_frame_width() const;
  float get_frame_height() const;
  // Get top-left, top-right, bottom-right and bottom-left uv coordinates
  std::array<glm::vec2, 4> get_frame_coords(const int frame) const;
  /* void set_custom_uv (const glm::vec2& uv, const float width, const float height); */

  // Load texture metadata from a json file
  void load_data(const std::string& filepath);

  // Convert a game id to a texture frame known from a metadata file
  const FrameData& id_to_frame(const uint32_t id, const std::string& type) const;

 private:
  struct PairHash
  {
    std::size_t operator()(std::pair<uint32_t, std::string> const& p) const
    {
      return std::hash<uint32_t>()(p.first) + std::hash<std::string>()(p.second);
    };
  };

  using FrameDataMap = std::unordered_map<std::pair<uint32_t, std::string>, FrameData, PairHash>;

  FrameDataMap m_frame_data;
  std::string m_filepath{};
  std::string m_data_filepath{};
  const int m_horizontal_frames;
  const int m_vertical_frames;
  unsigned int m_id = 0;
  int m_width = 0;
  int m_height = 0;

  void m_load_empty();
};
}  // namespace dl
