#pragma once

namespace dl
{
struct HomeFloor
{
  entt::entity home_entity = entt::null;
};

template <typename Archive>
void serialize(Archive& archive, HomeFloor& floor)
{
  archive(floor.home_entity);
}
}  // namespace dl
