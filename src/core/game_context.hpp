#pragma once

#include "./asset_manager.hpp"
#include "./clock.hpp"

namespace dl
{
class SceneManager;
class Display;
class InputManager;
class Camera;
class Renderer;
struct Clock;

struct GameContext
{
  Display* display;
  AssetManager* asset_manager;
  Renderer* renderer;
  Camera* camera;
  SceneManager* scene_manager;
  Clock* clock;
  std::shared_ptr<InputManager> input_manager;
};
}  // namespace dl
