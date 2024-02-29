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
class Renderer;
struct Clock;

struct GameContext
{
  Display* display = nullptr;
  AssetManager* asset_manager = nullptr;
  Camera* camera = nullptr;
  SceneManager* scene_manager = nullptr;
  Clock* clock = nullptr;
  WorldMetadata world_metadata;
  entt::registry* registry = nullptr;
  Renderer* renderer = nullptr;
};
}  // namespace dl
