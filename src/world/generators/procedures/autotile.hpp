#pragma once

#include <cstdint>
#include <vector>

#include "core/maths/random.hpp"
#include "core/maths/vector.hpp"
#include "world/cell.hpp"
#include "world/chunk.hpp"
#include "world/generators/procedures/node.hpp"

namespace dl
{
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

  void set_source(TileProcedureNode* source);

  void set_neighbor(BlockType neighbor);

  void set_bitmask_values(std::array<uint32_t, 16> bitmask_values);

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

  void set_source(TileProcedureNode* source);

  void set_neighbor(BlockType neighbor);

  void set_bitmask_values(std::array<uint32_t, 47> bitmask_values);

  void apply(TileProcedureData& data) override;

 private:
  uint32_t m_get_bitmask(TileProcedureData& data);
};
}  // namespace dl

