#pragma once

#include <map>
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include "./batch2d.hpp"

namespace dl
{
class ShaderProgram;
class ViewCamera;
class AssetManager;
class Sprite;
class Text;
class Texture;

class Renderer
{
public:
  Renderer (AssetManager& asset_manager);
  void init (const std::string& layer_id);
  void batch (const std::string& layer_id, const std::shared_ptr<Sprite>& sprite, const double x, const double y, const double z);
  void batch_text (const std::string& layer_id, Text& text, const double x, const double y, const double z);
  void finalize (const std::string& layer_id);
  void render (const ViewCamera& camera);
  void batch_sprites (entt::registry& registry);
  void add_layer (const std::string& layer_id, const std::string shader_id);
  std::shared_ptr<Texture> get_texture (const std::string& resource_id);

private:
  using LayerMap = std::map<std::string, std::shared_ptr<Batch2D>>;

  AssetManager& m_asset_manager;
  LayerMap m_layers;
};

}
