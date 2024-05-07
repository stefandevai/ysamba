#include "world/generators/tile_procedure_manager.hpp"
#include "world/generators/procedures/autotile.hpp"

namespace dl
{
void TileProcedureManager::init()
{
  if (m_has_initialized)
  {
    return;
  }

  auto set_front_face = std::make_unique<SetFrontFace>();
  set_front_face->set_front_face_id(39);

  auto autotile = std::make_unique<AutotileFourSidesHorizontal>();
  autotile->set_source(set_front_face.get());
  autotile->set_bitmask_values({38, 37, 34, 33, 30, 29, 26, 25, 36, 35, 32, 31, 28, 27, 24, 23});

  auto choose = std::make_unique<ChooseByUniformDistribution>();
  choose->set_source(autotile.get());
  choose->set_top_face_input(38);
  choose->set_candidates({
      {43, 0.008, ChooseByUniformDistribution::PlacementType::Terrain},
      {44, 0.008, ChooseByUniformDistribution::PlacementType::Terrain},
      {45, 0.008, ChooseByUniformDistribution::PlacementType::Terrain},
      {46, 0.008, ChooseByUniformDistribution::PlacementType::Terrain},
      {47, 0.008, ChooseByUniformDistribution::PlacementType::Terrain},
      {136, 0.01, ChooseByUniformDistribution::PlacementType::Terrain},
      {137, 0.01, ChooseByUniformDistribution::PlacementType::Terrain},
      {138, 0.01, ChooseByUniformDistribution::PlacementType::Terrain},
      {47, 0.002, ChooseByUniformDistribution::PlacementType::Terrain},
      {7, 0.003, ChooseByUniformDistribution::PlacementType::Decoration},
      {8, 0.001, ChooseByUniformDistribution::PlacementType::Decoration},
      {55, 0.003, ChooseByUniformDistribution::PlacementType::Decoration},
      {56, 0.001, ChooseByUniformDistribution::PlacementType::Decoration},
      {57, 0.001, ChooseByUniformDistribution::PlacementType::Decoration},
      {58, 0.001, ChooseByUniformDistribution::PlacementType::Decoration},
      {59, 0.001, ChooseByUniformDistribution::PlacementType::Decoration},
      {60, 0.001, ChooseByUniformDistribution::PlacementType::Decoration},
      {50, 0.002, ChooseByUniformDistribution::PlacementType::Decoration},
      {61, 0.002, ChooseByUniformDistribution::PlacementType::Decoration},
  });

  auto identity_procedure = std::make_unique<IdentityProcedure>();

  block_procedures[static_cast<int>(BlockType::None)] = identity_procedure.get();
  block_procedures[static_cast<int>(BlockType::Water)] = choose.get();
  block_procedures[static_cast<int>(BlockType::Grass)] = choose.get();
  block_procedures[static_cast<int>(BlockType::Basalt)] = choose.get();
  block_procedures[static_cast<int>(BlockType::Sand)] = choose.get();
  block_procedures[static_cast<int>(BlockType::Decoration)] = identity_procedure.get();

  m_procedures.push_back(std::move(identity_procedure));
  m_procedures.push_back(std::move(choose));
  m_procedures.push_back(std::move(autotile));
  m_procedures.push_back(std::move(set_front_face));

  m_has_initialized = true;
}

TileProcedureNode* TileProcedureManager::get_by_block(const BlockType block_type)
{
  return block_procedures[static_cast<int>(block_type)];
}

TileProcedureNode* TileProcedureManager::get_by_id(const uint32_t id)
{
  return procedures.at(id);
}
}  // namespace dl
