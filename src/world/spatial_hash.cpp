#include "./spatial_hash.hpp"

#include <spdlog/spdlog.h>

#include <entt/entity/entity.hpp>

#include "ecs/components/position.hpp"

namespace dl
{
SpatialHash::SpatialHash() : m_cell_dimension(0) {}

SpatialHash::SpatialHash(const uint32_t cell_dimension) { load(cell_dimension); }

void SpatialHash::load(const uint32_t cell_dimension)
{
  assert(cell_dimension >= 1);
  m_cell_dimension = cell_dimension;
}

uint32_t SpatialHash::add(const entt::entity object, const int x, const int y, const int z)
{
  const auto key = m_get_key(x, y, z);
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

uint32_t SpatialHash::update(const entt::entity object, const int x, const int y, const int z, const uint32_t key)
{
  const auto new_key = m_get_key(x, y, z);

  if (new_key == key)
  {
    return key;
  }

  remove(object, key);
  m_hash.emplace(new_key, object);

  return new_key;
}

std::vector<entt::entity> SpatialHash::get(const int x, const int y, const int z) const
{
  std::vector<entt::entity> objects{};
  const auto key = m_get_key(x, y, z);

  // for (const auto key : search_keys)
  // {
  auto range = m_hash.equal_range(key);
  for (auto i = range.first; i != range.second; ++i)
  {
    objects.push_back(i->second);
  }
  // }

  return objects;
}

bool SpatialHash::has(entt::entity entity, const int x, const int y, const int z) const
{
  const auto key = m_get_key(x, y, z);

  auto range = m_hash.equal_range(key);
  for (auto i = range.first; i != range.second; ++i)
  {
    if (i->second == entity)
    {
      return true;
    }
  }

  return false;
}

std::vector<entt::entity> SpatialHash::get_if(const Vector3i& position, TestFunction test_function) const
{
  std::vector<entt::entity> objects{};
  const auto key = m_get_key(position.x, position.y, position.z);

  auto range = m_hash.equal_range(key);
  for (auto i = range.first; i != range.second; ++i)
  {
    if (test_function(i->second))
    {
      objects.push_back(i->second);
    }
  }

  return objects;
}

uint32_t SpatialHash::m_get_key(const int x, const int y, const int z) const
{
  const uint32_t grid_x = static_cast<uint32_t>(x / m_cell_dimension);
  const uint32_t grid_y = static_cast<uint32_t>(y / m_cell_dimension);
  const uint32_t grid_z = static_cast<uint32_t>(z / m_cell_dimension);

  return (grid_x * 73856093) ^ (grid_y * 19349663) ^ (grid_z * 83492791);
}

constexpr std::array<int, 3> lookup_index{-1, 0, 1};

std::array<uint32_t, 27> SpatialHash::m_get_search_keys(const int x, const int y, const int z) const
{
  std::array<uint32_t, 27> keys{};

  int i = 0;

  for (const auto x_index : lookup_index)
  {
    for (const auto y_index : lookup_index)
    {
      for (const auto z_index : lookup_index)
      {
        keys[i]
            = m_get_key(x + x_index * m_cell_dimension, y + y_index * m_cell_dimension, z + z_index * m_cell_dimension);
        ++i;
      }
    }
  }

  return keys;
}
}  // namespace dl
