#pragma once

#include <unordered_map>
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include "./batch2d.hpp"

namespace dl
{
class ShaderProgram;
class ViewCamera;
class AssetManager;
class Sprite;

class Renderer
{
public:
  Renderer (AssetManager& asset_manager);
  void init ();
  void batch (const std::shared_ptr<Sprite>& sprite, const double x, const double y, const double z);
  void finalize ();
  void render (const ViewCamera& camera);
  void batch_sprites (entt::registry& registry);

private:
  enum class ShaderType
  {
    WORLD2D,
    GUI,
  };
  using ShaderMap = std::unordered_map<ShaderType, std::shared_ptr<ShaderProgram>>;

  AssetManager& m_asset_manager;
  Batch2D m_world_batch;
  Batch2D m_text_batch;
  ShaderMap m_shaders{
      {ShaderType::WORLD2D, nullptr},
      {ShaderType::GUI, nullptr},
  };

private:
  void m_init_assets();
};

}
