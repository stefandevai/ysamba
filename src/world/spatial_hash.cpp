#include "./spatial_hash.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/entity.hpp>

#include "ecs/components/position.hpp"

namespace dl
{
SpatialHash::SpatialHash() : m_width(0), m_height(0), m_cell_dimension(0) {}

SpatialHash::SpatialHash(const uint32_t width, const uint32_t height, const uint32_t cell_dimension)
{
  load(width, height, cell_dimension);
}

void SpatialHash::load(const uint32_t width, const uint32_t height, const uint32_t cell_dimension)
{
  assert(cell_dimension >= 1);

  m_width = width;
  m_height = height;
  m_cell_dimension = cell_dimension;
  m_horizontal_cells = m_width / cell_dimension;
  m_vertical_cells = m_height / cell_dimension;
  m_number_of_cells = m_horizontal_cells * m_vertical_cells;
}

uint32_t SpatialHash::add(const entt::entity object, const int x, const int y)
{
  const auto key = m_get_key(x, y);
  m_hash.emplace(key, object);
  return key;
}

void SpatialHash::remove(const entt::entity object, const uint32_t key)
{
  auto range = m_hash.equal_range(key);
  for (auto i = range.first; i != range.second; ++i)
  {
    if (i->second == object)
    {
      m_hash.erase(i);
      return;
    }
  }
}

uint32_t SpatialHash::update(const entt::entity object, const int x, const int y, const uint32_t key)
{
  const auto new_key = m_get_key(x, y);

  if (new_key == key)
  {
    return key;
  }

  remove(object, key);
  m_hash.emplace(new_key, object);
  return key;
}

std::vector<entt::entity> SpatialHash::get(const int x, const int y)
{
  std::vector<entt::entity> objects;
  const auto search_keys = m_get_search_keys(x, y);

  for (const auto& key : search_keys)
  {
    auto range = m_hash.equal_range(key);
    for (auto i = range.first; i != range.second; ++i)
    {
      objects.push_back(i->second);
    }
  }

  return objects;
}

uint32_t SpatialHash::m_get_key(const int x, const int y)
{
  const auto grid_x = x / m_cell_dimension;
  const auto grid_y = y / m_cell_dimension;
  return grid_y * m_horizontal_cells + grid_x;
}

std::vector<uint32_t> SpatialHash::m_get_search_keys(const int x, const int y)
{
  std::vector<uint32_t> keys;

  const auto grid_x = x / m_cell_dimension;
  const auto grid_y = y / m_cell_dimension;

  keys.push_back(grid_y * m_horizontal_cells + grid_x);

  // Add adjacent cells if they exist
  if (grid_x > 0)
  {
    keys.push_back(grid_y * m_horizontal_cells + grid_x - 1);
    if (grid_y > 0)
    {
      keys.push_back((grid_y - 1) * m_horizontal_cells + grid_x - 1);
    }
    if (grid_y < m_vertical_cells)
    {
      keys.push_back((grid_y + 1) * m_horizontal_cells + grid_x - 1);
    }
  }
  if (grid_x < m_horizontal_cells)
  {
    keys.push_back(grid_y * m_horizontal_cells + grid_x + 1);

    if (grid_y > 0)
    {
      keys.push_back((grid_y - 1) * m_horizontal_cells + grid_x + 1);
    }
    if (grid_y < m_vertical_cells)
    {
      keys.push_back((grid_y + 1) * m_horizontal_cells + grid_x + 1);
    }
  }
  if (grid_y > 0)
  {
    keys.push_back((grid_y - 1) * m_horizontal_cells + grid_x);
  }
  if (grid_y < m_vertical_cells)
  {
    keys.push_back((grid_y + 1) * m_horizontal_cells + grid_x);
  }

  return keys;
}
}  // namespace dl
