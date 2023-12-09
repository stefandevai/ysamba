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
class AssetManager;
class Sprite;
class MultiSprite;
class Text;
class Texture;

class Renderer
{
 public:
  Renderer(AssetManager& asset_manager);

  void batch(const uint32_t batch_id, Sprite* sprite, const double x, const double y, const double z);
  void batch(const uint32_t batch_id, MultiSprite* multi_sprite, const double x, const double y, const double z);
  void batch(const uint32_t batch_id, Text& text, const double x, const double y, const double z);
  void batch(const uint32_t batch_id, const Quad* quad, const double x, const double y, const double z);
  void render();
  void add_batch(const uint32_t batch_id, const std::string& shader_id, const int priority = 0);
  void add_batch(Batch* batch);
  void push_matrix(const uint32_t batch_id, const glm::mat4& matrix);
  const glm::mat4 pop_matrix(const uint32_t batch_id);
  const glm::mat4& peek_matrix(const uint32_t batch_id);
  void set_projection_matrix(const glm::mat4& projection_matrix) { m_projection_matrix = projection_matrix; }

  std::shared_ptr<Texture> get_texture(const std::string& resource_id);
  Batch* get_batch(const uint32_t batch_id) const { return m_batches.at(batch_id).get(); }
  bool has_batch(const uint32_t batch_id) const { return m_batches.contains(batch_id); }

 private:
  using LayerMap = std::map<uint32_t, std::unique_ptr<Batch>>;

  AssetManager& m_asset_manager;
  LayerMap m_batches;
  std::vector<Batch*> m_ordered_batches;
  glm::mat4 m_default_model_matrix = glm::mat4(1.0f);
  glm::mat4 m_default_view_matrix =
      glm::lookAt(glm::vec3{0.f, 0.f, 500.f}, glm::vec3{0.f, 0.f, -1.f}, glm::vec3{0.0f, 1.0f, 0.0f});
  glm::mat4 m_projection_matrix = glm::mat4{1.f};
};

}  // namespace dl
