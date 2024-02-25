#pragma once

#include <webgpu/wgpu.h>

#include <array>
#include <glm/vec2.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/maths/vector.hpp"
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

  struct PairHash
  {
    std::size_t operator()(std::pair<uint32_t, std::string> const& p) const
    {
      return std::hash<uint32_t>()(p.first) + std::hash<std::string>()(p.second);
    };
  };

  using Map = std::unordered_map<std::pair<uint32_t, std::string>, FrameData, PairHash>;
};

class Texture
{
 public:
  bool has_loaded = false;

  WGPUTexture texture;
  WGPUTextureView view;

  Texture(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels = 4);
  // Create full sized texture
  Texture(const std::string& filepath);
  // Create texture atlas
  Texture(const std::string& filepath,
          const int horizontal_frames,
          const int vertical_frames,
          const std::string& data_filepath = "");
  ~Texture();

  static Texture dummy(const WGPUDevice device);

  void load(const WGPUDevice device);
  void load(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels);
  inline unsigned int get_id() const { return m_id; }
  inline const Vector2i& get_size() const { return m_size; }
  inline int get_horizontal_frames() const { return m_horizontal_frames; }
  inline int get_vertical_frames() const { return m_vertical_frames; }
  inline const Vector2i& get_frame_size() const { return m_frame_size; }
  // Get top-left, top-right, bottom-right and bottom-left uv coordinates
  std::array<glm::vec2, 4> get_frame_coords(const int frame) const;
  /* void set_custom_uv (const glm::vec2& uv, const float width, const float height); */

  // Load texture metadata from a json file
  void load_metadata(const std::string& filepath);

  // Convert a game id to a texture frame known from a metadata file
  const FrameData& id_to_frame(const uint32_t id, const std::string& type) const;

 private:
  FrameData::Map m_frame_data;
  std::string m_filepath{};
  std::string m_data_filepath{};
  const int m_horizontal_frames = 1;
  const int m_vertical_frames = 1;
  unsigned int m_id = 0;
  Vector2i m_size{};
  Vector2i m_frame_size{};

  void m_load_empty();
};
}  // namespace dl
