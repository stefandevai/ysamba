/* #include "./renderer.hpp" */

/* #include "graphics/batch2d.hpp" */
/* #include "graphics/batch_quad.hpp" */
/* #include "graphics/camera.hpp" */
/* #include "graphics/shader_loader.hpp" */
/* #include "graphics/shader_program.hpp" */
/* #include "graphics/sprite.hpp" */
/* #include "graphics/text.hpp" */
/* #include "graphics/texture.hpp" */
/* #include "core/asset_manager.hpp" */

/* namespace dl::ui */
/* { */
/* UIRenderer::UIRenderer(AssetManager& asset_manager) : m_asset_manager(asset_manager) {} */

/* void UIRenderer::add_layer(const std::string& layer_id, */
/*                          const std::string shader_id, */
/*                          const LayerType layer_type, */
/*                          const bool ignore_camera, */
/*                          const int priority) */
/* { */
/*   const auto shader = m_asset_manager.get<ShaderProgram>(shader_id); */
/*   std::shared_ptr<Layer> layer; */

/*   switch (layer_type) */
/*   { */
/*   case LayerType::Sprite: */
/*     layer.reset(new Batch2D(shader, priority)); */
/*     break; */
/*   case LayerType::Quad: */
/*     layer.reset(new BatchQuad(shader, priority)); */
/*     break; */
/*   } */

/*   layer->set_ignore_camera(ignore_camera); */
/*   m_layers.emplace(layer_id, layer); */
/*   m_ordered_layers.push_back(layer); */

/*   std::sort(m_ordered_layers.begin(), m_ordered_layers.end(), [](const auto& lhs, const auto& rhs) { */
/*     return lhs->priority < rhs->priority; */
/*   }); */
/* } */

/* std::shared_ptr<Texture> UIRenderer::get_texture(const std::string& resource_id) */
/* { */
/*   return m_asset_manager.get<Texture>(resource_id); */
/* } */

/* void UIRenderer::init(const std::string& layer_id) { m_layers.at(layer_id)->init_emplacing(); } */

/* void UIRenderer::batch( */
/*     const std::string& layer_id, const std::shared_ptr<Sprite>& sprite, const double x, const double y, const double
 * z) */
/* { */
/*   const auto& layer = std::dynamic_pointer_cast<Batch2D>(m_layers.at(layer_id)); */
/*   assert(layer != nullptr); */

/*   // Load texture if it has not been loaded */
/*   if (sprite->texture == nullptr) */
/*   { */
/*     sprite->texture = m_asset_manager.get<Texture>(sprite->resource_id); */
/*   } */

/*   layer->emplace(sprite, x, y, z); */
/* } */

/* void UIRenderer::batch(const std::string& layer_id, Text& text, const double x, const double y, const double z) */
/* { */
/*   const auto& layer = std::dynamic_pointer_cast<Batch2D>(m_layers.at(layer_id)); */
/*   assert(layer != nullptr); */

/*   if (!text.get_has_initialized()) */
/*   { */
/*     text.initialize(m_asset_manager); */
/*   } */
/*   else if (!text.get_is_static()) */
/*   { */
/*     text.update(); */
/*   } */

/*   for (auto& character : text.characters) */
/*   { */
/*     if (character.sprite == nullptr) */
/*     { */
/*       continue; */
/*     } */
/*     if (character.sprite->texture == nullptr) */
/*     { */
/*       character.sprite->texture = m_asset_manager.get<Texture>(character.sprite->resource_id); */
/*     } */

/*     layer->emplace(character.sprite, character.x + x, character.y + y, z); */
/*   } */
/* } */

/* void UIRenderer::batch( */
/*     const std::string& layer_id, const std::shared_ptr<Quad>& quad, const double x, const double y, const double z)
 */
/* { */
/*   const auto& layer = std::dynamic_pointer_cast<BatchQuad>(m_layers.at(layer_id)); */
/*   assert(layer != nullptr); */

/*   layer->emplace(quad, x, y, z); */
/* } */

/* void UIRenderer::finalize(const std::string& layer_id) { m_layers.at(layer_id)->finalize_emplacing(); } */

/* void UIRenderer::render(const Camera& camera) */
/* { */
/*   glm::mat4 model_matrix = glm::mat4(1.0f); */

/*   for (const auto& layer : m_ordered_layers) */
/*   { */
/*     if (!layer->get_should_render()) */
/*     { */
/*       continue; */
/*     } */

/*     const auto& shader = layer->shader; */
/*     assert(shader != nullptr); */
/*     shader->use(); */

/*     if (layer->get_ignore_camera()) */
/*     { */
/*       shader->set_mat_4("mvp", camera.get_projection_matrix() * camera.get_default_view_matrix() * model_matrix); */
/*     } */
/*     else */
/*     { */
/*       shader->set_mat_4("mvp", camera.get_projection_matrix() * camera.get_view_matrix() * model_matrix); */
/*     } */

/*     layer->render(); */
/*   } */
/* } */
/* }  // namespace dl */