#pragma once

#include <memory>

#include "./camera_inspector.hpp"
#include "./chunk_debugger.hpp"
#include "./general_info.hpp"
#include "./render_editor.hpp"

union SDL_Event;
struct SDL_Window;
typedef void* SDL_GLContext;

namespace dl
{
class DebugTools
{
 public:
  bool open = true;

  DebugTools() = default;
  ~DebugTools();

  DebugTools(DebugTools const&) = delete;
  void operator=(DebugTools const&) = delete;

  static DebugTools& get_instance();
  void init(SDL_Window* window, SDL_GLContext& context);
  void process_event(SDL_Event* event);
  void update();
  void render();

  // Custom widgets
  void init_general_info(GameContext& context);
  void init_camera_inspector(Camera& camera);
  void init_render_editor(RenderSystem& render);
  void init_chunk_debugger(Gameplay& gameplay);

 private:
  static std::unique_ptr<DebugTools> m_instance;
  bool m_has_initialized = false;

  std::unique_ptr<GeneralInfo> m_general_info = nullptr;
  std::unique_ptr<CameraInspector> m_camera_inspector = nullptr;
  std::unique_ptr<RenderEditor> m_render_editor = nullptr;
  std::unique_ptr<ChunkDebugger> m_chunk_debugger = nullptr;

  void m_update_menu_bar();
};

}  // namespace dl
