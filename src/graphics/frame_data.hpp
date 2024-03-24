#pragma once

namespace dl
{
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
}  // namespace dl
