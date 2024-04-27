#pragma once

#include <vector>
#include <cstdint>
#include "core/maths/vector.hpp"
#include "core/maths/random.hpp"
#include "world/cell.hpp"
#include "world/chunk.hpp"

namespace dl
{
struct TileProcedureData
{
  Cell& cell;
  Vector3i& cell_position;
  Vector3i& padded_size;
  std::vector<BlockType>& terrain;
};

class TileProcedureNode
{
public:
  TileProcedureNode() = default;
  virtual ~TileProcedureNode() = default;

  virtual void apply(TileProcedureData& data) = 0;
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

  void set_source(TileProcedureNode* source)
  {
    this->source = source;
  }

  void set_neighbor(BlockType neighbor)
  {
    this->neighbor = neighbor;
  }

  void set_bitmask_values(std::array<uint32_t, 16> bitmask_values)
  {
    this->bitmask_values = std::move(bitmask_values);
  }

  void apply(TileProcedureData& data) override
  {
    if (source != nullptr)
    {
      source->apply(data);
    }

    const auto bitmask = m_get_bitmask(data);

    data.cell.top_face = bitmask_values[bitmask];
  }

private:
  uint32_t m_get_bitmask(TileProcedureData data)
  {
    uint32_t bitmask = 0;
    const auto terrain = data.terrain;
    const auto& padded_size = data.padded_size;
    const auto& position = data.cell_position;

    // Top
    if (position.y > 0 && terrain[position.z * padded_size.x * padded_size.y + (position.y - 1) * padded_size.x + position.x] == neighbor)
    {
      bitmask |= DL_EDGE_TOP;
    }
    // Right
    if (position.x < padded_size.x - 1
        && terrain[position.z * padded_size.x * padded_size.y + position.y * padded_size.x + position.x + 1] == neighbor)
    {
      bitmask |= DL_EDGE_RIGHT;
    }
    // Bottom
    if (position.y < padded_size.y - 1
        && terrain[position.z * padded_size.x * padded_size.y + (position.y + 1) * padded_size.x + position.x] == neighbor)
    {
      bitmask |= DL_EDGE_BOTTOM;
    }
    // Left
    if (position.x > 0 && terrain[position.z * padded_size.x * padded_size.y + position.y * padded_size.x + position.x - 1] == neighbor)
    {
      bitmask |= DL_EDGE_LEFT;
    }

    return bitmask;
  }
};

class SetFrontFace : public TileProcedureNode
{
public:
  TileProcedureNode* source = nullptr;
  uint32_t front_face_id = 0;

  void set_source(TileProcedureNode* source)
  {
    this->source = source;
  }

  void set_front_face_id(uint32_t front_face_id)
  {
    this->front_face_id = front_face_id;
  }

  void apply(TileProcedureData& data) override
  {
    if (source != nullptr)
    {
      source->apply(data);
    }

    data.cell.front_face = front_face_id;
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

  void set_source(TileProcedureNode* source)
  {
    this->source = source;
  }

  void set_candidates(std::vector<Candidate> candidates)
  {
    this->candidates = std::move(candidates);
  }

  void set_top_face_input(uint32_t top_face_input)
  {
    this->top_face_input = top_face_input;
  }

  void apply(TileProcedureData& data) override
  {
    if (source != nullptr)
    {
      source->apply(data);
    }

    if (data.cell.top_face != top_face_input)
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
          data.cell.top_face = candidate.value;
        }
        else
        {
          data.cell.top_face_decoration = candidate.value;
        }
        return;
      }
    }
  }
};
}
