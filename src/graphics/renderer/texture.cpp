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
// Create full sized texture
Texture::Texture(const std::string& filepath) : m_filepath(filepath) {}

// Create texture atlas
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

// Load texture from data on constructor
Texture::Texture(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels)
{
  load(device, data, size, channels);
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
  static constexpr unsigned char dummy_data = 0;
  Texture dummy_texture{device, &dummy_data, Vector2i{1, 1}, 1};
  return dummy_texture;
}

void Texture::load(const WGPUDevice device)
{
  assert(!m_filepath.empty() && "Texture filepath is empty");

  int width = 0;
  int height = 0;
  int channels = 0;
  unsigned char* image_data;

  // image_data = stbi_load (filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  image_data = stbi_load(m_filepath.c_str(), &width, &height, &channels, 0);

  if (image_data == nullptr)
  {
    spdlog::critical("Failed to load texture: {}", m_filepath);
    return;
  }

  load(device, image_data, Vector2i{width, height}, channels);
  stbi_image_free(image_data);
}

void Texture::load(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels)
{
  m_size = size;

  // If we have horizontal and vertical frames, calculate frame size
  if (m_horizontal_frames > 0 && m_vertical_frames > 0)
  {
    m_frame_size = Vector2i{m_size.x / m_horizontal_frames, m_size.y / m_vertical_frames};
  }

  // Load metadata
  if (m_data_filepath != "")
  {
    load_metadata(m_data_filepath);
  }

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
      .size = {static_cast<uint32_t>(m_size.x), static_cast<uint32_t>(m_size.y), 1},
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
      queue, &destination, data, m_size.x * m_size.y * channels * sizeof(unsigned char), &source, &textureDesc.size);

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

// Get top-left, top-right, bottom-right and bottom-left uv coordinates
std::array<glm::vec2, 4> Texture::get_frame_coords(const int frame, const int width, const int height) const
{
  const float frame_width = 1.0f / static_cast<float>(m_horizontal_frames);
  const float frame_height = 1.0f / static_cast<float>(m_vertical_frames);

  const int max_frames = m_horizontal_frames * m_vertical_frames;
  const float frame_x = static_cast<float>(frame % m_horizontal_frames);
  const float frame_y = static_cast<float>((frame % max_frames) / m_horizontal_frames);

  const float top_left_x = frame_width * frame_x;
  const float top_left_y = frame_height * frame_y;
  const float bottom_right_x = frame_width * (frame_x + width);
  const float bottom_right_y = frame_height * (frame_y + height);

  return std::array<glm::vec2, 4>{
      glm::vec2{top_left_x, top_left_y},
      glm::vec2{bottom_right_x, top_left_y},
      glm::vec2{bottom_right_x, bottom_right_y},
      glm::vec2{top_left_x, bottom_right_y},
  };
}

const FrameData& Texture::id_to_frame(const uint32_t id, const std::string& type) const
{
  return m_frame_data.at(std::make_pair(id, type));
}

void Texture::load_metadata(const std::string& filepath)
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
    const auto& sprite_type_str = item["sprite_type"].get<std::string>();

    FrameData frame_data{};

    if (item.contains("frame"))
    {
      // frame_data.frame = item["frame"].get<uint32_t>();
      frame_data.faces[DL_RENDER_FACE_TOP] = item["frame"].get<uint32_t>();
    }
    else if (item.contains("faces") && item.contains("default_face"))
    {
      const auto default_face = item["default_face"].get<std::string>();

      if (default_face == "top")
      {
        frame_data.default_face = DL_RENDER_FACE_TOP;
      }
      else if (default_face == "front")
      {
        frame_data.default_face = DL_RENDER_FACE_FRONT;
      }
      else if (default_face == "back")
      {
        frame_data.default_face = DL_RENDER_FACE_BACK;
      }
      else if (default_face == "bottom")
      {
        frame_data.default_face = DL_RENDER_FACE_BOTTOM;
      }
      else if (default_face == "left")
      {
        frame_data.default_face = DL_RENDER_FACE_LEFT;
      }
      else if (default_face == "right")
      {
        frame_data.default_face = DL_RENDER_FACE_RIGHT;
      }
      else if (default_face == "center_horizontal")
      {
        frame_data.default_face = DL_RENDER_FACE_CENTER_HORIZONTAL;
      }
      else if (default_face == "center_vertical")
      {
        frame_data.default_face = DL_RENDER_FACE_CENTER_VERTICAL;
      }

      const auto& faces = item["faces"];

      if (faces.contains("top"))
      {
        frame_data.faces[DL_RENDER_FACE_TOP] = faces["top"].get<uint32_t>();
      }
      if (faces.contains("front"))
      {
        frame_data.faces[DL_RENDER_FACE_FRONT] = faces["front"].get<uint32_t>();
      }
      if (faces.contains("back"))
      {
        frame_data.faces[DL_RENDER_FACE_BACK] = faces["back"].get<uint32_t>();
      }
      if (faces.contains("bottom"))
      {
        frame_data.faces[DL_RENDER_FACE_BOTTOM] = faces["bottom"].get<uint32_t>();
      }
      if (faces.contains("left"))
      {
        frame_data.faces[DL_RENDER_FACE_LEFT] = faces["left"].get<uint32_t>();
      }
      if (faces.contains("right"))
      {
        frame_data.faces[DL_RENDER_FACE_RIGHT] = faces["right"].get<uint32_t>();
      }
      if (faces.contains("center_horizontal"))
      {
        frame_data.faces[DL_RENDER_FACE_CENTER_HORIZONTAL] = faces["center_horizontal"].get<uint32_t>();
      }
      if (faces.contains("center_vertical"))
      {
        frame_data.faces[DL_RENDER_FACE_CENTER_VERTICAL] = faces["center_vertical"].get<uint32_t>();
      }
    }
    else
    {
      spdlog::critical("Frame data is missing for game id: {}", game_id);
    }

    if (sprite_type_str == "single")
    {
      frame_data.sprite_type = SpriteType::Single;
    }
    else if (sprite_type_str == "multiple")
    {
      frame_data.sprite_type = SpriteType::Multiple;
    }

    // if (item.contains("angle"))
    // {
    //   if (item["angle"] == "orthogonal")
    //   {
    //     frame_data.angle = FrameAngle::Orthogonal;
    //   }
    //   else
    //   {
    //     frame_data.angle = FrameAngle::Parallel;
    //   }
    // }
    // else
    // {
    //   frame_data.angle = FrameAngle::Parallel;
    // }

    // if (item.contains("front_face_id"))
    // {
    //   frame_data.front_face_id = item["front_face_id"].get<std::uint32_t>();
    // }
    // else
    // {
    //   frame_data.front_face_id = game_id;
    // }

    if (frame_data.sprite_type == SpriteType::Multiple)
    {
      frame_data.width = item["width"].get<uint32_t>();
      frame_data.height = item["height"].get<uint32_t>();
      frame_data.anchor_x = item["anchor_x"].get<int>();
      frame_data.anchor_y = item["anchor_y"].get<int>();

      if (type == "tile")
      {
        frame_data.pattern = item["pattern"].get<std::vector<uint32_t>>();
        frame_data.pattern_width = item["pattern_width"].get<uint32_t>();
        frame_data.pattern_height = item["pattern_height"].get<uint32_t>();
      }
    }

    m_frame_data[std::make_pair(game_id, type)] = frame_data;
  }

  has_metadata = true;
}

}  // namespace dl
