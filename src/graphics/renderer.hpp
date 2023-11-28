#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <string>
#include <vector>

#include "./batch.hpp"
#include "./quad.hpp"

namespace dl
{
class ShaderProgram;
class Camera;
class AssetManager;
class Sprite;
class Text;
class Texture;

class Renderer
{
 public:
  Renderer(AssetManager& asset_manager);

  void batch(const std::string& layer_id,
             const std::shared_ptr<Sprite>& sprite,
             const double x,
             const double y,
             const double z);
  void batch(const std::string& layer_id, Text& text, const double x, const double y, const double z);
  void batch(
      const std::string& layer_id, const std::shared_ptr<Quad>& quad, const double x, const double y, const double z);
  void render(const Camera& camera);
  void add_layer(const std::string& layer_id,
                 const std::string shader_id,
                 const bool ignore_camera = false,
                 const int priority = 0);
  void enable_depth_test();
  void disable_depth_test();

  std::shared_ptr<Texture> get_texture(const std::string& resource_id);
  inline bool has_layer(const std::string& layer_id) const { return m_layers.contains(layer_id); }

 private:
  using LayerMap = std::map<std::string, std::shared_ptr<Batch>>;

  AssetManager& m_asset_manager;
  std::vector<std::shared_ptr<Batch>> m_ordered_layers;
  LayerMap m_layers;
  glm::mat4 m_default_model_matrix = glm::mat4(1.0f);
  glm::mat4 m_default_view_matrix =
      glm::lookAt(glm::vec3{0.f, 0.f, 500.f}, glm::vec3{0.f, 0.f, -1.f}, glm::vec3{0.0f, 1.0f, 0.0f});
};

}  // namespace dl
