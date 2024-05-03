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
      new_terrain_id = bitmask_values[0];
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[1];
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[2];
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[3];
      break;
    case DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM
        | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[4];
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_LEFT | DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[5];
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[6];
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[7];
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP:
      new_terrain_id = bitmask_values[8];
      break;
    case DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[9];
      break;
    case DL_EDGE_LEFT | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[10];
      break;
    case DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[11];
      break;
    case DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[12];
      break;
    case DL_EDGE_TOP | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[13];
      break;
    case DL_EDGE_TOP:
      new_terrain_id = bitmask_values[14];
      break;
    case DL_EDGE_NONE:
      new_terrain_id = bitmask_values[15];
      break;
    case DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[16];
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[17];
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[18];
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[19];
      break;
    case DL_EDGE_BOTTOM | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP:
      new_terrain_id = bitmask_values[20];
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP
        | DL_EDGE_TOP_LEFT:
      new_terrain_id = bitmask_values[21];
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_RIGHT | DL_EDGE_TOP_RIGHT | DL_EDGE_TOP
        | DL_EDGE_TOP_LEFT:
      new_terrain_id = bitmask_values[22];
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_LEFT | DL_EDGE_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[23];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[24];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT
        | DL_EDGE_TOP_LEFT:
      new_terrain_id = bitmask_values[25];
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT
        | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[26];
      break;
    case DL_EDGE_TOP | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[27];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[28];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP:
      new_terrain_id = bitmask_values[29];
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[30];
      break;
    case DL_EDGE_TOP | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[31];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[32];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT:
      new_terrain_id = bitmask_values[33];
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[34];
      break;
    case DL_EDGE_TOP | DL_EDGE_LEFT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[35];
      break;
    case DL_EDGE_LEFT | DL_EDGE_BOTTOM | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[36];
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[37];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[38];
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP | DL_EDGE_RIGHT:
      new_terrain_id = bitmask_values[39];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[40];
      break;
    case DL_EDGE_LEFT | DL_EDGE_TOP_LEFT | DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM:
      new_terrain_id = bitmask_values[41];
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[42];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[43];
      break;
    case DL_EDGE_TOP | DL_EDGE_TOP_RIGHT | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[44];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_BOTTOM_LEFT | DL_EDGE_LEFT:
      new_terrain_id = bitmask_values[45];
      break;
    case DL_EDGE_TOP | DL_EDGE_RIGHT | DL_EDGE_BOTTOM | DL_EDGE_LEFT | DL_EDGE_TOP_LEFT:
      new_terrain_id = bitmask_values[46];
      break;
    }

    if (new_terrain_id == 0)
    {
      spdlog::warn("Could not find a matching tile for bitmask {}", bitmask);
    }

    data.cell->top_face = new_terrain_id;
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
    // Top Left
    if (position.x > 0 && position.y > 0
        && terrain[position.z * padded_size.x * padded_size.y + (position.y - 1) * padded_size.x + position.x - 1]
               == neighbor)
    {
      bitmask |= DL_EDGE_TOP_LEFT;
    }
    // Top Right
    if (position.x < padded_size.x - 1 && position.y > 0
        && terrain[position.z * padded_size.x * padded_size.y + (position.y - 1) * padded_size.x + position.x + 1]
               == neighbor)
    {
      bitmask |= DL_EDGE_TOP_RIGHT;
    }
    // Bottom Right
    if (position.x < padded_size.x - 1 && position.y < padded_size.y - 1
        && terrain[position.z * padded_size.x * padded_size.y + (position.y + 1) * padded_size.x + position.x + 1]
               == neighbor)
    {
      bitmask |= DL_EDGE_BOTTOM_RIGHT;
    }
    // Bottom Left
    if (position.x > 0 && position.y < padded_size.y - 1
        && terrain[position.z * padded_size.x * padded_size.y + (position.y + 1) * padded_size.x + position.x - 1]
               == neighbor)
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
