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

int SpatialHash::add(const entt::entity object, const int x, const int y, const int z)
{
  const auto key = m_get_key(x, y, z);
  m_hash.emplace(key, object);
  return key;
}

void SpatialHash::remove(const entt::entity object, const int key)
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

int SpatialHash::update(const entt::entity object, const int x, const int y, const int z, const int key)
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

int SpatialHash::m_get_key(const int x, const int y, const int z) const
{
  const auto grid_x = x / m_cell_dimension;
  const auto grid_y = y / m_cell_dimension;
  const auto grid_z = z / m_cell_dimension;

  const auto hash_x = std::hash<int>()(grid_x);
  const auto hash_y = std::hash<int>()(grid_y);
  const auto hash_z = std::hash<int>()(grid_z);

  return hash_x ^ hash_y ^ hash_z;
}

constexpr std::array<int, 3> lookup_index{-1, 0, 1};

std::array<int, 27> SpatialHash::m_get_search_keys(const int x, const int y, const int z) const
{
  std::array<int, 27> keys{};

  int i = 0;

  for (const auto x_index : lookup_index)
  {
    for (const auto y_index : lookup_index)
    {
      for (const auto z_index : lookup_index)
      {
        keys[i] =
            m_get_key(x + x_index * m_cell_dimension, y + y_index * m_cell_dimension, z + z_index * m_cell_dimension);
        ++i;
      }
    }
  }

  return keys;
}
}  // namespace dl
