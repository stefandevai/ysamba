#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <cstdint>

#include "world/cell.hpp"
#include "world/generators/tile_procedure.hpp"

namespace dl
{
class TileProcedureManager
{
public:
  inline static std::unordered_map<uint32_t, TileProcedureNode*> procedures{};
  inline static std::unordered_map<BlockType, TileProcedureNode*> block_procedures{};

  TileProcedureManager() = delete;

  static void init();
  static TileProcedureNode* get_by_block(BlockType block_type);
  static TileProcedureNode* get_by_id(uint32_t id);

private:
  inline static std::vector<std::unique_ptr<TileProcedureNode>> m_procedures{};
  inline static bool m_has_initialized = false;
};

}
