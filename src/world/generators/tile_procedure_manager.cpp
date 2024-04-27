#include "world/generators/tile_procedure_manager.hpp"

namespace dl
{
  void TileProcedureManager::init()
  {
    if (m_has_initialized)
    {
      return;
    }

    auto procedure = std::make_unique<AutotileFourSidesHorizontal>();
    procedure->set_bitmask_values({38, 37, 34, 33, 30, 29, 26, 25, 36, 35, 32, 31, 28, 27, 24, 23});

    block_procedures.insert({BlockType::Grass, procedure.get()});
    block_procedures.insert({BlockType::Basalt, procedure.get()});
    m_procedures.push_back(std::move(procedure));

    m_has_initialized = true;
  }

  TileProcedureNode* TileProcedureManager::get_by_block(const BlockType block_type)
  {
    return block_procedures.at(block_type);
  }

  TileProcedureNode* TileProcedureManager::get_by_id(const uint32_t id)
  {
    return procedures.at(id);
  }
}
