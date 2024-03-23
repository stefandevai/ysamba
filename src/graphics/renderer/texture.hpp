#pragma once

#include <webgpu/wgpu.h>

#include <array>
#include <glm/vec2.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/maths/vector.hpp"
#include "graphics/render_face.hpp"

namespace dl
{
enum class SpriteType
{
  Single,
  Multiple,
  Freeform,
};

struct FrameData
{
  uint32_t width = 1;
  uint32_t height = 1;
  uint32_t pattern_width = 0;
  uint32_t pattern_height = 0;
  int anchor_x = 0;
  int anchor_y = 0;
  RenderFace default_face = DL_RENDER_FACE_TOP;
  SpriteType sprite_type = SpriteType::Single;
  std::vector<uint32_t> pattern{};
  std::array<uint32_t, 8> faces{};

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
  bool has_metadata = false;

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

  // Loads after setting filepath
  void load(const WGPUDevice device);

  // Loads texture from data
  void load(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels);

  inline const Vector2i& get_size() const { return m_size; }
  inline const Vector2i& get_frame_size() const { return m_frame_size; }
  inline int get_horizontal_frames() const { return m_horizontal_frames; }
  inline int get_vertical_frames() const { return m_vertical_frames; }

  // Get top-left, top-right, bottom-right and bottom-left uv coordinates
  const std::array<glm::vec2, 4>& get_uv_coordinates(const uint32_t frame = 0) const;

  // Convert a game id to a texture frame known from a metadata file
  const FrameData& id_to_frame(const uint32_t id, const std::string& type) const;

 private:
  FrameData::Map m_frame_data;
  std::string m_filepath{};
  std::string m_data_filepath{};
  const int m_horizontal_frames = 1;
  const int m_vertical_frames = 1;
  Vector2i m_size{};
  Vector2i m_frame_size{};

  // Map where the key is the frame id and the value is an array of uv coordinates for a quad
  std::unordered_map<uint32_t, const std::array<glm::vec2, 4>> m_uv_coordinates{};

  // Load texture metadata from a json file
  void m_load_metadata(const std::string& filepath);

  // Generate uniform uv coordinates for the entire texture based on m_frame_size
  void m_generate_uv_coordinates();

  // Generate uv coordinates for a single frame
  void m_generate_uv_coordinate(const uint32_t frame_id, const FrameData& frame_data);

  // Calculate top-left, top-right, bottom-right and bottom-left uv coordinates
  std::array<glm::vec2, 4> m_calculate_uv_coordinates(const int frame = 0,
                                                      const int width = 1,
                                                      const int height = 1) const;
};
}  // namespace dl
