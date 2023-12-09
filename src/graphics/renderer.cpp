#include "./renderer.hpp"

#include <glad/glad.h>

#include "./multi_sprite.hpp"
#include "./shader_loader.hpp"
#include "./shader_program.hpp"
#include "./sprite.hpp"
#include "./text.hpp"
#include "./texture.hpp"
#include "core/asset_manager.hpp"

namespace dl
{
Renderer::Renderer(AssetManager& asset_manager) : m_asset_manager(asset_manager) {}

void Renderer::add_layer(const uint32_t layer_id, const std::string shader_id, const int priority)
{
  const auto shader = m_asset_manager.get<ShaderProgram>(shader_id);
  auto layer = std::make_unique<Batch>(shader, priority);

  m_ordered_layers.push_back(layer.get());
  m_layers.emplace(layer_id, std::move(layer));

  std::sort(m_ordered_layers.begin(), m_ordered_layers.end(), [](const auto& lhs, const auto& rhs) {
    return lhs->priority < rhs->priority;
  });
}

void Renderer::add_batch(Batch* batch)
{
  m_ordered_layers.push_back(batch);

  std::sort(m_ordered_layers.begin(), m_ordered_layers.end(), [](const auto& lhs, const auto& rhs) {
    return lhs->priority < rhs->priority;
  });
}

std::shared_ptr<Texture> Renderer::get_texture(const std::string& resource_id)
{
  return m_asset_manager.get<Texture>(resource_id);
}

void Renderer::batch(const uint32_t layer_id, Sprite* sprite, const double x, const double y, const double z)
{
  const auto& layer = m_layers.at(layer_id);

  // Load texture if it has not been loaded
  if (sprite->texture == nullptr)
  {
    sprite->texture = m_asset_manager.get<Texture>(sprite->resource_id);
  }

  layer->emplace(sprite, x, y, z);
}

void Renderer::batch(const uint32_t layer_id, MultiSprite* multi_sprite, const double x, const double y, const double z)
{
  const auto& layer = m_layers.at(layer_id);

  // Load texture if it has not been loaded
  if (multi_sprite->texture == nullptr)
  {
    multi_sprite->texture = m_asset_manager.get<Texture>(multi_sprite->resource_id);
  }

  layer->emplace(multi_sprite, x, y, z);
}

void Renderer::batch(const uint32_t layer_id, Text& text, const double x, const double y, const double z)
{
  const auto& layer = m_layers.at(layer_id);
  assert(layer != nullptr);

  layer->text(text, x, y, z);
}

void Renderer::batch(const uint32_t layer_id, const Quad* quad, const double x, const double y, const double z)
{
  m_layers.at(layer_id)->quad(quad, x, y, z);
}

void Renderer::render()
{
  for (const auto& layer : m_ordered_layers)
  {
    if (!layer->get_should_render())
    {
      continue;
    }

    if (layer->shader == nullptr)
    {
      layer->shader = m_asset_manager.get<ShaderProgram>(layer->shader_id);
    }

    if (layer->has_depth)
    {
      glEnable(GL_DEPTH_TEST);
    }
    else
    {
      glDisable(GL_DEPTH_TEST);
    }

    const auto& shader = layer->shader;
    assert(shader != nullptr);

    shader->use();
    shader->set_mat_4("mvp", m_projection_matrix * m_default_view_matrix);

    if (layer->has_scissor)
    {
      glEnable(GL_SCISSOR_TEST);
      glScissor(layer->scissor.x, layer->scissor.y, layer->scissor.z, layer->scissor.w);
    }
    else
    {
      glDisable(GL_SCISSOR_TEST);
    }

    layer->render();
  }

  glDisable(GL_SCISSOR_TEST);
}

void Renderer::enable_depth_test() { glEnable(GL_DEPTH_TEST); }

void Renderer::disable_depth_test() { glDisable(GL_DEPTH_TEST); }

void Renderer::push_matrix(const uint32_t layer_id, const glm::mat4& matrix)
{
  m_layers.at(layer_id)->push_matrix(matrix);
}

const glm::mat4 Renderer::pop_matrix(const uint32_t layer_id) { return m_layers.at(layer_id)->pop_matrix(); }

const glm::mat4& Renderer::peek_matrix(const uint32_t layer_id) { return m_layers.at(layer_id)->peek_matrix(); }
}  // namespace dl
