#pragma once

#include "./asset_manager.hpp"
#include "./clock.hpp"

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
};
}  // namespace dl
