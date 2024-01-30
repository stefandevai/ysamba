#pragma once

#include "./asset_manager2.hpp"
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
  AssetManager2* asset_manager;
  Camera* camera;
  SceneManager* scene_manager;
  Clock* clock;
  WorldMetadata world_metadata;
};
}  // namespace dl
