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

  void apply(TileProcedureData& data) override;

 private:
  uint32_t m_get_bitmask(TileProcedureData& data);
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

  void apply(TileProcedureData& data) override;

 private:
  uint32_t m_get_bitmask(TileProcedureData& data);
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

  void set_source(TileProcedureNode* source);

  void set_candidates(std::vector<Candidate> candidates);

  void set_top_face_input(uint32_t top_face_input);

  void apply(TileProcedureData& data) override;
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
