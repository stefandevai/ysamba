#include <entt/entity/registry.hpp>
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

void Renderer::add_layer (const std::string& layer_id, const std::string shader_id)
{
  m_layers.emplace (layer_id, std::make_shared<Batch2D> (m_asset_manager.get<ShaderProgram> (shader_id)));
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
  // Load texture if it has not been loaded
  if (sprite->texture == nullptr)
  {
    sprite->texture = m_asset_manager.get<Texture> (sprite->resource_id);
  }

  m_layers.at(layer_id)->emplace(sprite, x, y, z);
}

void Renderer::batch_text (const std::string& layer_id, Text& text, const double x, const double y, const double z)
{
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

    m_layers.at(layer_id)->emplace(character.sprite, character.x + x, character.y + y, z);
  }
  /* // Load texture if it has not been loaded */
  /* if (sprite->texture == nullptr) */
  /* { */
  /*   sprite->texture = m_asset_manager.get<Texture> (sprite->resource_id); */
  /* } */

  /* m_layers.at(layer_id)->emplace(sprite, x, y, z); */
}

void Renderer::finalize(const std::string& layer_id)
{
  m_layers.at(layer_id)->finalize_emplacing();
}

void Renderer::batch_sprites (entt::registry& registry)
{
  /* auto sprite_view = registry.view<component::Sprite, component::Position> (entt::exclude<component::Charcode>); */

  /* m_world_batch.init_emplacing(); */

  /* for (auto entity : sprite_view) */
  /* { */
  /*   auto& sprite = registry.get<component::Sprite> (entity); */

  /*   // Load texture if it has not been loaded */
  /*   if (sprite.texture == nullptr) */
  /*   { */
  /*     sprite.texture = m_asset_manager.get<Texture> (sprite.resource_id); */
  /*   } */

  /*   m_world_batch.emplace (registry, entity); */
  /* } */

  /* m_world_batch.finalize_emplacing(); */

  /* auto character_view = registry.view<component::Sprite, component::Position, component::Charcode>(); */
  /* m_text_batch.init_emplacing(); */

  /* for (auto entity : character_view) */
  /* { */
  /*   auto& sprite = registry.get<component::Sprite> (entity); */

  /*   // Load texture if it has not been loaded */
  /*   if (sprite.texture == nullptr) */
  /*   { */
  /*     sprite.texture = m_asset_manager.get<Texture> (sprite.resource_id); */
  /*   } */

  /*   m_text_batch.emplace (registry, entity); */
  /* } */
  /* m_text_batch.finalize_emplacing(); */
}

void Renderer::render (const ViewCamera& camera)
{
  glm::mat4 model_matrix = glm::mat4 (1.0f);

  for (const auto& p : m_layers)
  {
    const auto& shader = p.second->shader;
    shader->use();
    shader->set_mat_4 ("mvp", camera.get_projection_matrix() * camera.get_view_matrix() * model_matrix);
    p.second->render();
  }
}
}

