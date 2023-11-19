#pragma once

#include <map>
#include <string>
#include <tuple>

#include "./quad.hpp"

namespace dl
{
class ShaderProgram;
class Camera;
class AssetManager;
class Sprite;
class Text;
class Texture;
class Layer;

class Renderer
{
 public:
  enum class LayerType
  {
    Sprite,
    Quad,
  };

  Renderer(AssetManager& asset_manager);

  void init(const std::string& layer_id);
  void batch(const std::string& layer_id,
             const std::shared_ptr<Sprite>& sprite,
             const double x,
             const double y,
             const double z);
  void batch(const std::string& layer_id, Text& text, const double x, const double y, const double z);
  void batch(
      const std::string& layer_id, const std::shared_ptr<Quad>& quad, const double x, const double y, const double z);
  void finalize(const std::string& layer_id);
  void render(const Camera& camera);
  void add_layer(const std::string& layer_id,
                 const std::string shader_id,
                 const LayerType layer_type = LayerType::Sprite,
                 const bool ignore_camera = false);

  std::shared_ptr<Texture> get_texture(const std::string& resource_id);
  inline bool has_layer(const std::string& layer_id) const { return m_layers.contains(layer_id); }

 private:
  using LayerMap = std::map<std::string, std::shared_ptr<Layer>>;

  AssetManager& m_asset_manager;
  LayerMap m_layers;
};

}  // namespace dl
