#pragma once

#include <entt/fwd.hpp>

#include "./asset_manager.hpp"
#include "./clock.hpp"
#include "world/metadata.hpp"

namespace dl
{
class SceneManager;
class Display;
class Camera;
struct Clock;

struct GameContext
{
  Display* display;
  AssetManager* asset_manager;
  Camera* camera;
  SceneManager* scene_manager;
  Clock* clock;
  WorldMetadata world_metadata;
  entt::registry* registry;
};
}  // namespace dl
