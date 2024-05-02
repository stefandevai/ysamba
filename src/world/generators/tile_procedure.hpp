#pragma once

#include <cstdint>
#include <vector>

#include "core/maths/random.hpp"
#include "core/maths/vector.hpp"
#include "world/cell.hpp"
#include "world/chunk.hpp"

namespace dl
{
struct TileProcedureData
{
  Chunk* chunk;
  Cell* cell;
  Vector3i* cell_position;
  Vector3i* padded_size;
  std::vector<BlockType>* terrain;
};

class TileProcedureNode
{
 public:
  TileProcedureNode() = default;
  virtual ~TileProcedureNode() = default;

  virtual void apply(TileProcedureData& data) = 0;
};

class IdentityProcedure : public TileProcedureNode
{
 public:
  void apply(TileProcedureData& data) override { (void)data; }
};

typedef enum
{
  DL_EDGE_NONE = 0,
  DL_EDGE_TOP = 1,
  DL_EDGE_RIGHT = 2,
  DL_EDGE_BOTTOM = 4,
  DL_EDGE_LEFT = 8,
  DL_EDGE_TOP_LEFT = 16,
  DL_EDGE_TOP_RIGHT = 32,
  DL_EDGE_BOTTOM_RIGHT = 64,
  DL_EDGE_BOTTOM_LEFT = 128,
} Edge;

class AutotileFourSidesHorizontal : public TileProcedureNode
{
 public:
  TileProcedureNode* source = nullptr;
  BlockType neighbor = BlockType::None;
  std::array<uint32_t, 16> bitmask_values{};

  void set_source(TileProcedureNode* source) { this->source = source; }

  void set_neighbor(BlockType neighbor) { this->neighbor = neighbor; }

  void set_bitmask_values(std::array<uint32_t, 16> bitmask_values) { this->bitmask_values = std::move(bitmask_values); }

  void apply(TileProcedureData& data) override
  {
    if (source != nullptr)
    {
      source->apply(data);
    }

    const auto bitmask = m_get_bitmask(data);

    data.cell->top_face = bitmask_values[bitmask];
  }

 private:
  uint32_t m_get_bitmask(TileProcedureData& data)
  {
    uint32_t bitmask = 0;
    const auto& terrain = *data.terrain;
    const auto& padded_size = *data.padded_size;
    const auto& position = *data.cell_position;

    // Top
    if (position.y > 0
        && terrain[position.z * padded_size.x * padded_size.y + (position.y - 1) * padded_size.x + position.x]
               == neighbor)
    {
      bitmask |= DL_EDGE_TOP;
    }
    // Right
    if (position.x < padded_size.x - 1
        && terrain[position.z * padded_size.x * padded_size.y + position.y * padded_size.x + position.x + 1]
               == neighbor)
    {
      bitmask |= DL_EDGE_RIGHT;
    }
    // Bottom
    if (position.y < padded_size.y - 1
        && terrain[position.z * padded_size.x * padded_size.y + (position.y + 1) * padded_size.x + position.x]
               == neighbor)
    {
      bitmask |= DL_EDGE_BOTTOM;
    }
    // Left
    if (position.x > 0
        && terrain[position.z * padded_size.x * padded_size.y + position.y * padded_size.x + position.x - 1]
               == neighbor)
    {
      bitmask |= DL_EDGE_LEFT;
    }

    return bitmask;
  }
};

class AutotileEightSidesHorizontal : public TileProcedureNode
{
 public:
  TileProcedureNode* source = nullptr;
  BlockType neighbor = BlockType::None;
  std::array<uint32_t, 47> bitmask_values{};

  void set_source(TileProcedureNode* source) { this->source = source; }

  void set_neighbor(BlockType neighbor) { this->neighbor = neighbor; }

  void set_bitmask_values(std::array<uint32_t, 47> bitmask_values) { this->bitmask_values = std::move(bitmask_values); }

  void apply(TileProcedureData& data) override
  {
    if (source != nullptr)
    {
      source->apply(data);
    }

    const auto bitmask = m_get_bitmask(data);

    int new_terrain_id = 0;

    switch (bitmask)
    {
    case DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[0].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[1].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[2].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[3].value;
      break;
    case DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM
        | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = rule.output[4].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_LEFT | DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[5].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[6].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[7].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP:
      new_terrain_id = rule.output[8].value;
      break;
    case DL_EDGE_RIGHT:
      new_terrain_id = rule.output[9].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[10].value;
      break;
    case DL_EDGE_LEFT:
      new_terrain_id = rule.output[11].value;
      break;
    case DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[12].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[13].value;
      break;
    case DL_EDGE_TOP:
      new_terrain_id = rule.output[14].value;
      break;
    case DL_EDGE_NONE:
      new_terrain_id = rule.output[15].value;
      break;
    case DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[16].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[17].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[18].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[19].value;
      break;
    case DL_EDGE_BOTTOM | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP:
      new_terrain_id = rule.output[20].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP
        | DL_EDGE_TOP_LEFT:
      new_terrain_id = rule.output[21].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP
        | DL_EDGE_TOP_LEFT:
      new_terrain_id = rule.output[22].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_LEFT | DL_EDGE_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[23].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[24].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT
        | DL_EDGE_TOP_LEFT:
      new_terrain_id = rule.output[25].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT
        | DL_EDGE_LEFT:
      new_terrain_id = rule.output[26].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = rule.output[27].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[28].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP:
      new_terrain_id = rule.output[29].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_LEFT:
      new_terrain_id = rule.output[30].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_LEFT:
      new_terrain_id = rule.output[31].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[32].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT:
      new_terrain_id = rule.output[33].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
      new_terrain_id = rule.output[34].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[35].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[36].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[37].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = rule.output[38].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP | DL_EDGE_RIGHT:
      new_terrain_id = rule.output[39].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
      new_terrain_id = rule.output[40].value;
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = rule.output[41].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = rule.output[42].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
      new_terrain_id = rule.output[43].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
      new_terrain_id = rule.output[44].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = rule.output[45].value;
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT:
      new_terrain_id = rule.output[46].value;
      break;
    }

    if (new_terrain_id == 0)
    {
      spdlog::warn("Could not find a matching tile for bitmask {}", bitmask);
    }

    values.top_face = new_terrain_id;

    data.cell->top_face = bitmask_values[bitmask];
  }

 private:
  uint32_t m_get_bitmask(TileProcedureData& data)
  {
    uint32_t bitmask = 0;

    // Top
    if (y > 0 && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x] == source)
    {
      bitmask |= DL_EDGE_TOP;
    }
    // Right
    if (x < m_padded_size.x - 1 && terrain[z * m_padded_size.x * m_padded_size.y + y * m_padded_size.x + x + 1] == source)
    {
      bitmask |= DL_EDGE_RIGHT;
    }
    // Bottom
    if (y < m_padded_size.y - 1
        && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x] == source)
    {
      bitmask |= DL_EDGE_BOTTOM;
    }
    // Left
    if (x > 0 && terrain[z * m_padded_size.x * m_padded_size.y + y * m_padded_size.x + x - 1] == source)
    {
      bitmask |= DL_EDGE_LEFT;
    }
    // Top Left
    if (x > 0 && y > 0 && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x - 1] == source)
    {
      bitmask |= DL_EDGE_TOP_LEFT;
    }
    // Top Right
    if (x < m_padded_size.x - 1 && y > 0
        && terrain[z * m_padded_size.x * m_padded_size.y + (y - 1) * m_padded_size.x + x + 1] == source)
    {
      bitmask |= DL_EDGE_TOP_RIGHT;
    }
    // Bottom Right
    if (x < m_padded_size.x - 1 && y < m_padded_size.y - 1
        && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x + 1] == source)
    {
      bitmask |= DL_EDGE_BOTTOM_RIGHT;
    }
    // Bottom Left
    if (x > 0 && y < m_padded_size.y - 1
        && terrain[z * m_padded_size.x * m_padded_size.y + (y + 1) * m_padded_size.x + x - 1] == source)
    {
      bitmask |= DL_EDGE_BOTTOM_LEFT;
    }

    if (!(bitmask & DL_EDGE_LEFT) || !(bitmask & DL_EDGE_TOP))
    {
      bitmask &= ~DL_EDGE_TOP_LEFT;
    }
    if (!(bitmask & DL_EDGE_LEFT) || !(bitmask & DL_EDGE_BOTTOM))
    {
      bitmask &= ~DL_EDGE_BOTTOM_LEFT;
    }
    if (!(bitmask & DL_EDGE_RIGHT) || !(bitmask & DL_EDGE_TOP))
    {
      bitmask &= ~DL_EDGE_TOP_RIGHT;
    }
    if (!(bitmask & DL_EDGE_RIGHT) || !(bitmask & DL_EDGE_BOTTOM))
    {
      bitmask &= ~DL_EDGE_BOTTOM_RIGHT;
    }

    return bitmask;
  }
};

class SetFrontFace : public TileProcedureNode
{
 public:
  TileProcedureNode* source = nullptr;
  uint32_t front_face_id = 0;

  void set_source(TileProcedureNode* source) { this->source = source; }

  void set_front_face_id(uint32_t front_face_id) { this->front_face_id = front_face_id; }

  void apply(TileProcedureData& data) override
  {
    if (source != nullptr)
    {
      source->apply(data);
    }

    data.cell->front_face = front_face_id;
  }
};

class ChooseByUniformDistribution : public TileProcedureNode
{
 public:
  enum class PlacementType
  {
    Terrain,
    Decoration,
  };

  struct Candidate
  {
    int value;
    double probability;
    PlacementType placement;
  };

  TileProcedureNode* source = nullptr;
  uint32_t top_face_input = 0;
  std::vector<Candidate> candidates{};

  void set_source(TileProcedureNode* source) { this->source = source; }

  void set_candidates(std::vector<Candidate> candidates) { this->candidates = std::move(candidates); }

  void set_top_face_input(uint32_t top_face_input) { this->top_face_input = top_face_input; }

  void apply(TileProcedureData& data) override
  {
    if (source != nullptr)
    {
      source->apply(data);
    }

    if (data.cell->top_face != top_face_input)
    {
      return;
    }

    const auto probability = random::get_real();
    double cumulative_probability = 0.0;

    for (const auto& candidate : candidates)
    {
      cumulative_probability += candidate.probability;

      if (probability < cumulative_probability)
      {
        if (candidate.placement == PlacementType::Terrain)
        {
          data.cell->top_face = candidate.value;
        }
        else
        {
          data.cell->top_face_decoration = candidate.value;
        }
        return;
      }
    }
  }
};

class GenerateTerrainChunk : public TileProcedureNode
{
 public:
  TileProcedureNode* source = nullptr;
  int padding = 1;

  void set_source(TileProcedureNode* source);
  void set_padding(const int padding);
  void apply(TileProcedureData& data) override;
};
}  // namespace dl
