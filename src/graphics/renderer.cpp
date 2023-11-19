#include <entt/entity/registry.hpp>
#include "./layer.hpp"
#include "./batch2d.hpp"
#include "./batch_quad.hpp"
#include "./renderer.hpp"
#include "./shader_program.hpp"
#include "./camera.hpp"
#include "./texture.hpp"
#include "./shader_loader.hpp"
#include "./sprite.hpp"
#include "./text.hpp"
#include "../core/asset_manager.hpp"

namespace dl
{
Renderer::Renderer (AssetManager& asset_manager) : m_asset_manager (asset_manager) { }

void Renderer::add_layer (const std::string& layer_id, const std::string shader_id, const LayerType layer_type, const bool ignore_camera)
{
  const auto shader = m_asset_manager.get<ShaderProgram> (shader_id);
  std::shared_ptr<Layer> layer;

  switch(layer_type)
  {
    case LayerType::Sprite:
      layer.reset(new Batch2D(shader));
      break;
    case LayerType::Quad:
      layer.reset(new BatchQuad(shader));
      break;
  }

  layer->set_ignore_camera(ignore_camera);
  m_layers.emplace (layer_id, layer);
}

std::shared_ptr<Texture> Renderer::get_texture (const std::string& resource_id)
{
  return m_asset_manager.get<Texture> (resource_id);
}

void Renderer::init(const std::string& layer_id)
{
  m_layers.at(layer_id)->init_emplacing();
}

void Renderer::batch (const std::string& layer_id, const std::shared_ptr<Sprite>& sprite, const double x, const double y, const double z)
{
  const auto& layer = std::dynamic_pointer_cast<Batch2D>(m_layers.at(layer_id));
  assert(layer != nullptr);

  // Load texture if it has not been loaded
  if (sprite->texture == nullptr)
  {
    sprite->texture = m_asset_manager.get<Texture> (sprite->resource_id);
  }

  layer->emplace(sprite, x, y, z);
}

void Renderer::batch (const std::string& layer_id, Text& text, const double x, const double y, const double z)
{
  const auto& layer = std::dynamic_pointer_cast<Batch2D>(m_layers.at(layer_id));
  assert(layer != nullptr);

  if (!text.get_has_initialized())
  {
    text.initialize(m_asset_manager);
  }

  for (auto& character : text.characters)
  {
    if (character.sprite == nullptr)
    {
      continue;
    }
    if (character.sprite->texture == nullptr)
    {
      character.sprite->texture = m_asset_manager.get<Texture> (character.sprite->resource_id);
    }

    layer->emplace(character.sprite, character.x + x, character.y + y, z);
  }
}

void Renderer::batch (const std::string& layer_id, const std::shared_ptr<Quad>& quad, const double x, const double y, const double z)
{
  const auto& layer = std::dynamic_pointer_cast<BatchQuad>(m_layers.at(layer_id));
  assert(layer != nullptr);

  layer->emplace(quad, x, y, z);
}

void Renderer::finalize(const std::string& layer_id)
{
  m_layers.at(layer_id)->finalize_emplacing();
}

void Renderer::render (const ViewCamera& camera)
{
  glm::mat4 model_matrix = glm::mat4 (1.0f);

  for (const auto& layer : m_layers)
  {
    const auto& shader = layer.second->shader;
    assert(shader != nullptr);
    shader->use();

    if (layer.second->get_ignore_camera())
    {
      shader->set_mat_4 ("mvp", camera.get_projection_matrix() * camera.get_default_view_matrix() * model_matrix);
    }
    else
    {
      shader->set_mat_4 ("mvp", camera.get_projection_matrix() * camera.get_view_matrix() * model_matrix);
    }

    layer.second->render();
  }
}
}

