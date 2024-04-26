#pragma once

#include <vector>
#include <memory>
#include "world/cell.hpp"

namespace dl
{
struct TileProcedureData
{
  std::vector<BlockType>& terrain;
}

struct TileProcedureNode
{
  TileProcedureData* data = nullptr;
  std::vector<std::unique_ptr<TileProcedureNode>> children{};

  TileProcedureNode() = default;
  virtual ~TileProcedureNode() = default;

  virtual void apply() = 0;

  void set_data(TileProcedureData* data)
  {
    this->data = data;
  }

  void run()
  {
    apply();

    for (auto& child : children)
    {
      child->set_data(data);
      child->apply();
      child->run();
    }
  }
}

struct IdentityProcedure : public TileProcedureNode
{
  void apply() override
  {
    // Do nothing
  }
}

struct AutotileFourSides : public TileProcedureNode
{
  BlockType neighbor;

  void apply() override
  {
  }
}

}
