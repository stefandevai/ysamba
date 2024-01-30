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
class Camera;

class Renderer
{
 public:
  Renderer(AssetManager& asset_manager);

  void batch(const uint32_t batch_id, Sprite* sprite, const double x, const double y, const double z);
  void batch(const uint32_t batch_id, MultiSprite* multi_sprite, const double x, const double y, const double z);
  void batch(const uint32_t batch_id, Text& text, const double x, const double y, const double z);
  void batch(const uint32_t batch_id, const Quad* quad, const double x, const double y, const double z);
  void render(const Camera& camera);
  const Texture* get_texture(const uint32_t resource_id);
  void add_batch(const uint32_t batch_id, const uint32_t shader_id, const int priority = 0);
  void add_batch(Batch* batch);
  void push_matrix(const uint32_t batch_id, const glm::mat4& matrix);
  const glm::mat4 pop_matrix(const uint32_t batch_id);
  const glm::mat4& peek_matrix(const uint32_t batch_id);

  Batch* get_batch(const uint32_t batch_id) const { return m_batches.at(batch_id).get(); }
  bool has_batch(const uint32_t batch_id) const { return m_batches.contains(batch_id); }

 private:
  using LayerMap = std::map<uint32_t, std::unique_ptr<Batch>>;

  AssetManager& m_asset_manager;
  LayerMap m_batches;
  std::vector<Batch*> m_ordered_batches;
};

}  // namespace dl
