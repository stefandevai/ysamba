#pragma once

#include <memory>

#include "./camera_editor.hpp"
#include "./general_info.hpp"

union SDL_Event;
struct SDL_Window;
typedef void* SDL_GLContext;

namespace dl
{
class Camera;

class Debug
{
 public:
  Debug() = default;
  ~Debug();

  Debug(Debug const&) = delete;
  void operator=(Debug const&) = delete;

  static Debug& get_instance();
  void init(SDL_Window* window, SDL_GLContext& context);
  void process_event(SDL_Event* event);
  void update();
  void render();

  // Custom widgets
  void init_general_info(GameContext& context);
  void init_camera_editor(Camera& camera);

 private:
  static std::unique_ptr<Debug> m_instance;
  bool m_has_initialized = false;

  std::unique_ptr<GeneralInfo> m_general_info = nullptr;
  std::unique_ptr<CameraEditor> m_camera_editor = nullptr;
};

}  // namespace dl
