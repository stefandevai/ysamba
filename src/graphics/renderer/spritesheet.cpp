#include "./spritesheet.hpp"

#include <spdlog/spdlog.h>

#include "core/json.hpp"

namespace dl
{
// Create full sized texture
Spritesheet::Spritesheet(const std::string& filepath) : texture(std::make_unique<Texture>(filepath)) {}

// Create texture atlas
Spritesheet::Spritesheet(const std::string& filepath, const std::string& data_filepath)
    : texture(std::make_unique<Texture>(filepath)), m_data_filepath(data_filepath)
{
}

// Load texture from data on constructor
Spritesheet::Spritesheet(const WGPUDevice device, const unsigned char* data, const Vector2i& size, const int channels)
    : texture(std::make_unique<Texture>(device, data, size, channels))
{
  m_generate_uv_coordinates();
}

void Spritesheet::load(const WGPUDevice device)
{
  texture->load(device);

  // Load metadata
  if (m_data_filepath != "")
  {
    m_load_metadata(m_data_filepath);
  }
  else
  {
    m_generate_uv_coordinates();
  }

  has_loaded = true;
}

// Get top-left, top-right, bottom-right and bottom-left uv coordinates
std::array<glm::vec2, 4> Spritesheet::m_calculate_uv_coordinates(const int frame,
                                                                 const int width,
                                                                 const int height) const
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

const std::array<glm::vec2, 4>& Spritesheet::get_uv_coordinates(const uint32_t frame) const
{
  return m_uv_coordinates.at(frame);
}

const FrameData& Spritesheet::id_to_frame(const uint32_t id, const std::string& type) const
{
  return m_frame_data.at(std::make_pair(id, type));
}

void Spritesheet::m_load_metadata(const std::string& filepath)
{
  JSON json{filepath};

  if (!json.object.contains("frames"))
  {
    spdlog::warn("Trying to load tileset data without frames: {}", filepath);
    return;
  }

  if (json.object.contains("tile_width") && json.object.contains("tile_height"))
  {
    const auto tile_width = json.object["tile_width"].get<int>();
    const auto tile_height = json.object["tile_height"].get<int>();

    const auto& size = texture->size;

    m_horizontal_frames = size.x / tile_width;
    m_vertical_frames = size.y / tile_height;

    m_frame_size = Vector2i{size.x / m_horizontal_frames, size.y / m_vertical_frames};
  }

  const auto items = json.object["frames"].get<std::vector<nlohmann::json>>();

  for (const auto& item : items)
  {
    const auto type = item["type"].get<std::string>();
    const auto game_id = item["game_id"].get<uint32_t>();
    const auto& sprite_type_str = item["sprite_type"].get<std::string>();

    FrameData frame_data{};

    if (sprite_type_str == "single")
    {
      frame_data.sprite_type = SpriteType::Single;
    }
    else if (sprite_type_str == "multiple")
    {
      frame_data.sprite_type = SpriteType::Multiple;
    }

    if (frame_data.sprite_type == SpriteType::Multiple)
    {
      frame_data.width = item["width"].get<uint32_t>();
      frame_data.height = item["height"].get<uint32_t>();
      frame_data.anchor_x = item["anchor_x"].get<int>();
      frame_data.anchor_y = item["anchor_y"].get<int>();

      if (type == "tile")
      {
        frame_data.pattern = item["pattern"].get<std::vector<uint32_t>>();
        const auto pattern_width = item["pattern_width"].get<int>();
        const auto pattern_height = item["pattern_height"].get<int>();
        frame_data.pattern_size = Vector2i{pattern_width, pattern_height};
      }
    }

    if (item.contains("frame"))
    {
      const auto frame_id = item["frame"].get<uint32_t>();
      frame_data.faces[DL_RENDER_FACE_TOP] = frame_id;
      m_generate_uv_coordinate(frame_id, frame_data);
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
        const auto frame_id = faces["top"].get<uint32_t>();
        frame_data.faces[DL_RENDER_FACE_TOP] = frame_id;
        m_generate_uv_coordinate(frame_id, frame_data);
      }
      if (faces.contains("front"))
      {
        const auto frame_id = faces["front"].get<uint32_t>();
        frame_data.faces[DL_RENDER_FACE_FRONT] = frame_id;
        m_generate_uv_coordinate(frame_id, frame_data);
      }
      if (faces.contains("back"))
      {
        const auto frame_id = faces["back"].get<uint32_t>();
        frame_data.faces[DL_RENDER_FACE_BACK] = frame_id;
        m_generate_uv_coordinate(frame_id, frame_data);
      }
      if (faces.contains("bottom"))
      {
        const auto frame_id = faces["bottom"].get<uint32_t>();
        frame_data.faces[DL_RENDER_FACE_BOTTOM] = frame_id;
        m_generate_uv_coordinate(frame_id, frame_data);
      }
      if (faces.contains("left"))
      {
        const auto frame_id = faces["left"].get<uint32_t>();
        frame_data.faces[DL_RENDER_FACE_LEFT] = frame_id;
        m_generate_uv_coordinate(frame_id, frame_data);
      }
      if (faces.contains("right"))
      {
        const auto frame_id = faces["right"].get<uint32_t>();
        frame_data.faces[DL_RENDER_FACE_RIGHT] = frame_id;
        m_generate_uv_coordinate(frame_id, frame_data);
      }
      if (faces.contains("center_horizontal"))
      {
        const auto frame_id = faces["center_horizontal"].get<uint32_t>();
        frame_data.faces[DL_RENDER_FACE_CENTER_HORIZONTAL] = frame_id;
        m_generate_uv_coordinate(frame_id, frame_data);
      }
      if (faces.contains("center_vertical"))
      {
        const auto frame_id = faces["center_vertical"].get<uint32_t>();
        frame_data.faces[DL_RENDER_FACE_CENTER_VERTICAL] = frame_id;
        m_generate_uv_coordinate(frame_id, frame_data);
      }
    }
    else
    {
      spdlog::critical("Frame data is missing for game id: {}", game_id);
    }

    m_frame_data[std::make_pair(game_id, type)] = frame_data;
  }

  has_metadata = true;
}

void Spritesheet::m_generate_uv_coordinates()
{
  for (int i = 0; i < m_horizontal_frames * m_vertical_frames; ++i)
  {
    m_uv_coordinates.insert({static_cast<uint32_t>(i), m_calculate_uv_coordinates(i)});
  }
}

void Spritesheet::m_generate_uv_coordinate(const uint32_t frame_id, const FrameData& frame_data)
{
  m_uv_coordinates.insert({frame_id, m_calculate_uv_coordinates(frame_id, frame_data.width, frame_data.height)});
}

}  // namespace dl
