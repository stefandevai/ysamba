#include "./renderer.hpp"

#include <glad/glad.h>

#include "./camera.hpp"
#include "./multi_sprite.hpp"
#include "./shader_loader.hpp"
#include "./shader_program.hpp"
#include "./sprite.hpp"
#include "./text.hpp"
#include "./texture.hpp"
#include "core/asset_manager.hpp"

namespace dl
{
Renderer::Renderer(AssetManager& asset_manager) : m_asset_manager(asset_manager) { glClearColor(0.f, 0.f, 0.f, 1.0f); }

void Renderer::add_batch(const uint32_t batch_id, const std::string& shader_id, const int priority)
{
  auto batch = std::make_unique<Batch>(shader_id, priority);

  m_ordered_batches.push_back(batch.get());
  std::sort(m_ordered_batches.begin(), m_ordered_batches.end(), [](const auto& lhs, const auto& rhs) {
    return lhs->priority < rhs->priority;
  });

  m_batches.emplace(batch_id, std::move(batch));
}

void Renderer::add_batch(Batch* batch)
{
  m_ordered_batches.push_back(batch);

  std::sort(m_ordered_batches.begin(), m_ordered_batches.end(), [](const auto& lhs, const auto& rhs) {
    return lhs->priority < rhs->priority;
  });
}

std::shared_ptr<Texture> Renderer::get_texture(const std::string& resource_id)
{
  return m_asset_manager.get<Texture>(resource_id);
}

void Renderer::batch(const uint32_t batch_id, Sprite* sprite, const double x, const double y, const double z)
{
  const auto& batch = m_batches.at(batch_id);

  // Load texture if it has not been loaded
  if (sprite->texture == nullptr)
  {
    sprite->texture = m_asset_manager.get<Texture>(sprite->resource_id);
  }

  batch->emplace(sprite, x, y, z);
}

void Renderer::batch(const uint32_t batch_id, MultiSprite* multi_sprite, const double x, const double y, const double z)
{
  const auto& batch = m_batches.at(batch_id);

  // Load texture if it has not been loaded
  if (multi_sprite->texture == nullptr)
  {
    multi_sprite->texture = m_asset_manager.get<Texture>(multi_sprite->resource_id);
  }

  batch->emplace(multi_sprite, x, y, z);
}

void Renderer::batch(const uint32_t batch_id, Text& text, const double x, const double y, const double z)
{
  const auto& batch = m_batches.at(batch_id);
  assert(batch != nullptr);

  batch->text(text, x, y, z);
}

void Renderer::batch(const uint32_t batch_id, const Quad* quad, const double x, const double y, const double z)
{
  m_batches.at(batch_id)->quad(quad, x, y, z);
}

void Renderer::render(const Camera& camera)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const auto& batch : m_ordered_batches)
  {
    if (batch->index_count == 0)
    {
      continue;
    }

    if (batch->shader == nullptr)
    {
      batch->shader = m_asset_manager.get<ShaderProgram>(batch->shader_id);
    }
    assert(batch->shader != nullptr);

    if (batch->has_blend)
    {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_BLEND);
    }
    else
    {
      glDisable(GL_BLEND);
    }
    if (batch->has_depth)
    {
      glEnable(GL_DEPTH_TEST);
    }
    else
    {
      glDisable(GL_DEPTH_TEST);
    }
    if (batch->has_scissor && batch->scissor.z - batch->scissor.x > 0 && batch->scissor.w - batch->scissor.y > 0)
    {
      glEnable(GL_SCISSOR_TEST);
      glScissor(batch->scissor.x, batch->scissor.y, batch->scissor.z, batch->scissor.w);
    }
    else
    {
      glDisable(GL_SCISSOR_TEST);
    }

    batch->shader->use();
    batch->shader->set_mat4("projection", camera.projection_matrix);
    batch->shader->set_mat4("view", batch->peek_matrix());

    batch->render();
  }

  glDisable(GL_SCISSOR_TEST);
}

void Renderer::push_matrix(const uint32_t batch_id, const glm::mat4& matrix)
{
  m_batches.at(batch_id)->push_matrix(matrix);
}

const glm::mat4 Renderer::pop_matrix(const uint32_t batch_id) { return m_batches.at(batch_id)->pop_matrix(); }

const glm::mat4& Renderer::peek_matrix(const uint32_t batch_id) { return m_batches.at(batch_id)->peek_matrix(); }
}  // namespace dl
