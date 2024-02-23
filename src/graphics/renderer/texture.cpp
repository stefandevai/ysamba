#include "./texture.hpp"

#include <spdlog/spdlog.h>

#include "core/json.hpp"

extern "C"
{
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

namespace dl
{
// Load single texture
Texture::Texture(const std::string& filepath) : m_filepath(filepath), m_horizontal_frames(1), m_vertical_frames(1) {}

// Load uniform texture atlas
Texture::Texture(const std::string& filepath,
                 const int horizontal_frames,
                 const int vertical_frames,
                 const std::string& data_filepath)
    : m_filepath(filepath),
      m_data_filepath(data_filepath),
      m_horizontal_frames(horizontal_frames),
      m_vertical_frames(vertical_frames)
{
}

// Load with raw data
Texture::Texture(const std::vector<unsigned char>& data, const int width, const int height)
    : m_horizontal_frames(1), m_vertical_frames(1)
{
  // load(data.data(), width, height, 4);
}

Texture::Texture(const int width, const int height)
    : m_horizontal_frames(1), m_vertical_frames(1), m_width(width), m_height(height)
{
  // m_load_empty();
}

Texture::~Texture()
{
  if (has_loaded)
  {
    wgpuTextureViewRelease(view);
    wgpuTextureDestroy(texture);
    wgpuTextureRelease(texture);
  }
}

Texture Texture::dummy(const WGPUDevice device)
{
  const std::vector<unsigned char> dummy_data = {0, 0, 0, 0};

  Texture dummy_texture{dummy_data, 1, 1};
  dummy_texture.load(device, dummy_data.data(), 1, 1, 4);

  return dummy_texture;
}

void Texture::load(const WGPUDevice device)
{
  if (m_data_filepath != "")
  {
    load_data(m_data_filepath);
  }

  int width = 0;
  int height = 0;
  int channels = 0;
  unsigned char* image_data;

  // image_data = stbi_load (filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  image_data = stbi_load(m_filepath.c_str(), &width, &height, &channels, 0);
  if (image_data == nullptr)
  {
    throw std::runtime_error("It wasn't possible to load " + m_filepath);
  }

  load(device, image_data, width, height, channels);

  stbi_image_free(image_data);
}

void Texture::load(
    const WGPUDevice device, const unsigned char* data, const int width, const int height, const int channels)
{
  m_width = width;
  m_height = height;

  const auto queue = wgpuDeviceGetQueue(device);

  WGPUTextureFormat format;
  switch (channels)
  {
  case 1:
    format = WGPUTextureFormat_R8Unorm;
    break;

  case 4:
    format = WGPUTextureFormat_RGBA8Unorm;
    break;

  default:
    format = WGPUTextureFormat_RGBA8Unorm;
    break;
  }

  WGPUTextureDescriptor textureDesc{
      .label = "WorldPipeline Texture",
      .dimension = WGPUTextureDimension_2D,
      .size = {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1},
      .mipLevelCount = 1,
      .sampleCount = 1,
      .format = format,
      .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
      .viewFormatCount = 0,
      .viewFormats = nullptr,
  };

  texture = wgpuDeviceCreateTexture(device, &textureDesc);
  assert(texture != nullptr);

  WGPUImageCopyTexture destination = {
      .texture = texture,
      .mipLevel = 0,
      .origin = {0, 0, 0},
      .aspect = WGPUTextureAspect_All,
  };

  WGPUTextureDataLayout source = {
      .offset = 0,
      .bytesPerRow = channels * textureDesc.size.width,
      .rowsPerImage = textureDesc.size.height,
  };

  wgpuQueueWriteTexture(
      queue, &destination, data, width * height * channels * sizeof(uint8_t), &source, &textureDesc.size);

  WGPUTextureViewDescriptor textureViewDesc{
      .label = "WorldPipeline Texture View",
      .aspect = WGPUTextureAspect_All,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .dimension = WGPUTextureViewDimension_2D,
      .format = textureDesc.format,
  };
  view = wgpuTextureCreateView(texture, &textureViewDesc);
  assert(view != nullptr);

  has_loaded = true;
}

// TODO: Implement irregular frame calculations
float Texture::get_frame_width() const { return (m_width / static_cast<float>(m_horizontal_frames)); }

// TODO: Implement irregular frame calculations
float Texture::get_frame_height() const { return (m_height / static_cast<float>(m_vertical_frames)); }

// TODO: Implement irregular frame calculations
// Get top-left, top-right, bottom-right and bottom-left uv coordinates
std::array<glm::vec2, 4> Texture::get_frame_coords(const int frame) const
{
  assert(m_width > 0 && m_height > 0);

  const auto frame_width = get_frame_width() / static_cast<float>(m_width);
  const auto frame_height = get_frame_height() / static_cast<float>(m_height);
  const int max_frames = m_horizontal_frames * m_vertical_frames;
  const float frame_x = static_cast<float>(frame % m_horizontal_frames);
  const float frame_y = static_cast<float>((frame % max_frames) / m_horizontal_frames);
  // Multiply the x coord of the frame in the texture atlas by the normalized value of the width one frame.
  const float top_left_x = frame_x * (m_width / static_cast<float>(m_horizontal_frames)) / m_width;
  // Multiply the y coord of the frame in the tile map by the normalized value of the height one frame.
  // Invert the value as the y axis is upwards for OpenGL
  const float top_left_y = frame_y * (m_height / static_cast<float>(m_vertical_frames)) / m_height;

  return std::array<glm::vec2, 4>{
      glm::vec2{top_left_x, top_left_y},
      glm::vec2{top_left_x + frame_width, top_left_y},
      glm::vec2{top_left_x + frame_width, top_left_y + frame_height},
      glm::vec2{top_left_x, top_left_y + frame_height},
  };
}

void Texture::m_load_empty()
{
  // GLenum err;
  // while ((err = glGetError()) != GL_NO_ERROR)
  // {
  //   spdlog::critical("Failed to create empty Texture. OpenGL error code: 0x{0:04x}", err);
  // }
  //
  // glGenTextures(1, &m_id);
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // glBindTexture(GL_TEXTURE_2D, m_id);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0x0000);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glBindTexture(GL_TEXTURE_2D, 0);
  //
  // while ((err = glGetError()) != GL_NO_ERROR)
  // {
  //   spdlog::critical("Failed to create empty Texture. OpenGL error code: 0x{0:04x}", err);
  // }
}

const FrameData& Texture::id_to_frame(const uint32_t id, const std::string& type) const
{
  return m_frame_data.at(std::make_pair(id, type));
}

void Texture::load_data(const std::string& filepath)
{
  JSON json{filepath};

  if (!json.object.contains("frames"))
  {
    spdlog::warn("Trying to load tileset data without frames: {}", filepath);
    return;
  }

  const auto items = json.object["frames"].get<std::vector<nlohmann::json>>();

  for (const auto& item : items)
  {
    const auto type = item["type"].get<std::string>();
    const auto game_id = item["game_id"].get<uint32_t>();

    FrameData frame_data{};

    frame_data.frame = item["frame"].get<uint32_t>();

    const auto& sprite_type_str = item["sprite_type"].get<std::string>();

    if (sprite_type_str == "single")
    {
      frame_data.sprite_type = SpriteType::Single;
    }
    else if (sprite_type_str == "multiple")
    {
      frame_data.sprite_type = SpriteType::Multiple;
    }

    if (item.contains("angle"))
    {
      if (item["angle"] == "orthogonal")
      {
        frame_data.angle = FrameAngle::Orthogonal;
      }
      else
      {
        frame_data.angle = FrameAngle::Parallel;
      }
    }
    else
    {
      frame_data.angle = FrameAngle::Parallel;
    }

    if (item.contains("front_face_id"))
    {
      frame_data.front_face_id = item["front_face_id"].get<std::uint32_t>();
    }
    else
    {
      frame_data.front_face_id = game_id;
    }

    if (frame_data.sprite_type == SpriteType::Multiple)
    {
      frame_data.width = item["width"].get<uint32_t>();
      frame_data.height = item["height"].get<uint32_t>();
      frame_data.pattern = item["pattern"].get<std::vector<uint32_t>>();
      frame_data.pattern_width = item["pattern_width"].get<uint32_t>();
      frame_data.pattern_height = item["pattern_height"].get<uint32_t>();
      frame_data.anchor_x = item["anchor_x"].get<int>();
      frame_data.anchor_y = item["anchor_y"].get<int>();
    }

    m_frame_data[std::make_pair(game_id, type)] = frame_data;
  }
}

}  // namespace dl
