#include <entt/entity/registry.hpp>
#include "./renderer.hpp"
#include "./config.hpp"
#include "./shader_program.hpp"
#include "./camera.hpp"
#include "./texture.hpp"
#include "./asset_manager.hpp"
#include "./shader_loader.hpp"
#include "./sprite.hpp"
/* #include "../ecs/components/position.hpp" */
/* #include "../ecs/components/text.hpp" */
/* #include "../ecs/components/charcode.hpp" */

namespace dl
{
Renderer::Renderer (AssetManager& asset_manager) : m_asset_manager (asset_manager) { m_init_assets(); }

void Renderer::init()
{
  m_world_batch.init_emplacing();
}

void Renderer::batch (const std::shared_ptr<Sprite>& sprite, const double x, const double y, const double z)
{
  // Load texture if it has not been loaded
  if (sprite->texture == nullptr)
  {
    sprite->texture = m_asset_manager.get<Texture> (sprite->resource_id);
  }

  m_world_batch.emplace (sprite, x, y, z);
}

void Renderer::finalize()
{
  m_world_batch.finalize_emplacing();
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
  // Render Sprites
  auto& shader2d = m_shaders[ShaderType::WORLD2D];
  shader2d->use();

  glm::mat4 model2d = glm::mat4 (1.0f);
  shader2d->set_mat_4 ("mvp", camera.get_projection_matrix() * camera.get_view_matrix() * model2d);

  m_world_batch.render (*shader2d);

  /* // Render GUI */
  /* auto& shader_gui = m_shaders[ShaderType::GUI]; */
  /* shader_gui->use(); */
  /* shader_gui->set_mat_4 ("mvp", camera.get_projection_matrix() * camera.get_view_matrix() * model2d); */
  /* m_text_batch.render (*shader_gui); */
}

void Renderer::m_init_assets()
{
  m_asset_manager.add<ShaderLoader> (WORLD2D_SHADER_NAME, WORLD2D_SHADER_PATH);
  m_shaders[ShaderType::WORLD2D] = m_asset_manager.get<ShaderProgram> (WORLD2D_SHADER_NAME);
  assert (m_shaders[ShaderType::WORLD2D] != nullptr);

/*   m_asset_manager.add<ShaderLoader> (GUI_SHADER_NAME, GUI_SHADER_PATH); */
/*   m_shaders[ShaderType::GUI] = m_asset_manager.get<ShaderProgram> (GUI_SHADER_NAME); */
/*   assert (m_shaders[ShaderType::GUI] != nullptr); */
}
}

