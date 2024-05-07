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
class IdentityProcedure : public TileProcedureNode
{
 public:
  void apply(TileProcedureData& data) override { (void)data; }
};

class SetFrontFace : public TileProcedureNode
{
 public:
  TileProcedureNode* source = nullptr;
  uint32_t front_face_id = 0;

  void set_source(TileProcedureNode* source);

  void set_front_face_id(uint32_t front_face_id);

  void apply(TileProcedureData& data);
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
