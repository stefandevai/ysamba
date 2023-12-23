#pragma once

#include <memory>

union SDL_Event;
struct SDL_Window;
typedef void* SDL_GLContext;

namespace dl
{
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

 private:
  bool m_has_initialized = false;
  static std::unique_ptr<Debug> m_instance;
};

}  // namespace dl
